/*
** $Id: liolib.c 3794 2010-08-03 07:02:06Z taco $
** Standard I/O (and system) library
** See Copyright Notice in lua.h
*/


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define liolib_c
#define LUA_LIB

#ifndef MINGW32
#  define LUA_USE_POSIX 1
#endif

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "ptexlib.h"

#define IO_INPUT	1
#define IO_OUTPUT	2

static const char *const fnames[] = { "input", "output" };


static int pushresult(lua_State * L, int i, const char *filename)
{
    int en = errno;             /* calls to Lua API may change this value */
    if (i) {
        lua_pushboolean(L, 1);
        return 1;
    } else {
        lua_pushnil(L);
        if (filename)
            lua_pushfstring(L, "%s: %s", filename, strerror(en));
        else
            lua_pushfstring(L, "%s", strerror(en));
        lua_pushinteger(L, en);
        return 3;
    }
}


static void fileerror(lua_State * L, int arg, const char *filename)
{
    lua_pushfstring(L, "%s: %s", filename, strerror(errno));
    luaL_argerror(L, arg, lua_tostring(L, -1));
}


#define topfile(L)	((FILE **)luaL_checkudata(L, 1, LUA_FILEHANDLE))


static int io_type(lua_State * L)
{
    void *ud;
    luaL_checkany(L, 1);
    ud = lua_touserdata(L, 1);
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_FILEHANDLE);
    if (ud == NULL || !lua_getmetatable(L, 1) || !lua_rawequal(L, -2, -1))
        lua_pushnil(L);         /* not a file */
    else if (*((FILE **) ud) == NULL)
        lua_pushliteral(L, "closed file");
    else
        lua_pushliteral(L, "file");
    return 1;
}


static FILE *tofile(lua_State * L)
{
    FILE **f = topfile(L);
    if (*f == NULL)
        luaL_error(L, "attempt to use a closed file");
    return *f;
}



/*
** When creating file handles, always creates a `closed' file handle
** before opening the actual file; so, if there is a memory error, the
** file is not left opened.
*/
static FILE **newfile(lua_State * L)
{
    FILE **pf = (FILE **) lua_newuserdata(L, sizeof(FILE *));
    *pf = NULL;                 /* file handle is currently `closed' */
    luaL_getmetatable(L, LUA_FILEHANDLE);
    lua_setmetatable(L, -2);
    return pf;
}


/*
** this function has a separated environment, which defines the
** correct __close for 'popen' files
*/
static int io_pclose(lua_State * L)
{
    FILE **p = topfile(L);
    int ok = lua_pclose(L, *p);
    *p = NULL;
    return pushresult(L, ok, NULL);
}


static int io_fclose(lua_State * L)
{
    FILE **p = topfile(L);
    int ok = (fclose(*p) == 0);
    *p = NULL;
    return pushresult(L, ok, NULL);
}


static int aux_close(lua_State * L)
{
    lua_getfenv(L, 1);
    lua_getfield(L, -1, "__close");
    return (lua_tocfunction(L, -1)) (L);
}


static int io_close(lua_State * L)
{
    if (lua_isnone(L, 1))
        lua_rawgeti(L, LUA_ENVIRONINDEX, IO_OUTPUT);
    tofile(L);                  /* make sure argument is a file */
    return aux_close(L);
}


static int io_gc(lua_State * L)
{
    FILE *f = *topfile(L);
    /* ignore closed files and standard files */
    if (f != NULL && f != stdin && f != stdout && f != stderr)
        aux_close(L);
    return 0;
}


static int io_tostring(lua_State * L)
{
    FILE *f = *topfile(L);
    if (f == NULL)
        lua_pushstring(L, "file (closed)");
    else
        lua_pushfstring(L, "file (%p)", f);
    return 1;
}


static int io_open(lua_State * L)
{
    const char *filename = luaL_checkstring(L, 1);
    const char *mode = luaL_optstring(L, 2, "r");
    FILE **pf = newfile(L);
    *pf = fopen(filename, mode);
    if (*pf == NULL)
        return pushresult(L, 0, filename);
    if (mode[0]=='r') 
        recorder_record_input(filename);
    else
        recorder_record_output(filename);
    return 1;
}

static int io_open_ro(lua_State * L)
{
    FILE **pf;
    const char *filename = luaL_checkstring(L, 1);
    const char *mode = luaL_optstring(L, 2, "r");
    if ((strcmp(mode, "r") != 0) && (strcmp(mode, "rb") != 0))
        return pushresult(L, 0, filename);
    pf = newfile(L);
    *pf = fopen(filename, mode);
    if (*pf == NULL)
        return pushresult(L, 0, filename);
    recorder_record_input(filename);
    return 1;
}





static int io_popen(lua_State * L)
{
    int ret = 1;
    char *safecmd = NULL;
    char *cmdname = NULL;
    int allow = 0;
    const char *cmd = luaL_checkstring(L, 1);
    const char *mode = luaL_optstring(L, 2, "r");
    FILE **pf = newfile(L);

    if (shellenabledp <= 0) {
        lua_pushnil(L);
        lua_pushliteral(L, "All command execution is disabled");
        return 2;
    }
    /* If restrictedshell == 0, any command is allowed. */
    if (restrictedshell == 0)
        allow = 1;
    else
        allow = shell_cmd_is_allowed(&cmd, &safecmd, &cmdname);

    if (allow == 1) {
        *pf = lua_popen(L, cmd, mode);
        ret = (*pf == NULL) ? pushresult(L, 0, cmd) : 1;
    } else if (allow == 2) {
        *pf = lua_popen(L, safecmd, mode);
        ret = (*pf == NULL) ? pushresult(L, 0, safecmd) : 1;
    } else if (allow == 0) {
        lua_pushnil(L);
        lua_pushliteral(L, "Command execution disabled via shell_escape='p'");
        ret = 2;
    } else {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad command line quoting");
        ret = 2;
    }
    return ret;
}


static int io_tmpfile(lua_State * L)
{
    FILE **pf = newfile(L);
    *pf = tmpfile();
    return (*pf == NULL) ? pushresult(L, 0, NULL) : 1;
}


static FILE *getiofile(lua_State * L, int findex)
{
    FILE *f;
    lua_rawgeti(L, LUA_ENVIRONINDEX, findex);
    f = *(FILE **) lua_touserdata(L, -1);
    if (f == NULL)
        luaL_error(L, "standard %s file is closed", fnames[findex - 1]);
    return f;
}


static int g_iofile(lua_State * L, int f, const char *mode)
{
    if (!lua_isnoneornil(L, 1)) {
        const char *filename = lua_tostring(L, 1);
        if (filename) {
            FILE **pf = newfile(L);
            *pf = fopen(filename, mode);
            if (*pf == NULL) {
                fileerror(L, 1, filename);
            } else {
              if (mode[0]=='r') 
                recorder_record_input(filename);
              else
                recorder_record_output(filename);
            }
        } else {
            tofile(L);          /* check that it's a valid file handle */
            lua_pushvalue(L, 1);
        }
        lua_rawseti(L, LUA_ENVIRONINDEX, f);
    }
    /* return current value */
    lua_rawgeti(L, LUA_ENVIRONINDEX, f);
    return 1;
}


static int io_input(lua_State * L)
{
    return g_iofile(L, IO_INPUT, "r");
}


static int io_output(lua_State * L)
{
    return g_iofile(L, IO_OUTPUT, "w");
}


static int io_readline(lua_State * L);


static void aux_lines(lua_State * L, int idx, int toclose)
{
    lua_pushvalue(L, idx);
    lua_pushboolean(L, toclose);        /* close/not close file when finished */
    lua_pushcclosure(L, io_readline, 2);
}


static int f_lines(lua_State * L)
{
    tofile(L);                  /* check that it's a valid file handle */
    aux_lines(L, 1, 0);
    return 1;
}


static int io_lines(lua_State * L)
{
    if (lua_isnoneornil(L, 1)) {        /* no arguments? */
        /* will iterate over default input */
        lua_rawgeti(L, LUA_ENVIRONINDEX, IO_INPUT);
        return f_lines(L);
    } else {
        const char *filename = luaL_checkstring(L, 1);
        FILE **pf = newfile(L);
        *pf = fopen(filename, "r");
        if (*pf == NULL)
            fileerror(L, 1, filename);
        else
            recorder_record_input(filename);
        aux_lines(L, lua_gettop(L), 1);
        return 1;
    }
}


/*
** {======================================================
** READ
** =======================================================
*/


static int read_number(lua_State * L, FILE * f)
{
    lua_Number d;
    if (fscanf(f, LUA_NUMBER_SCAN, &d) == 1) {
        lua_pushnumber(L, d);
        return 1;
    } else
        return 0;               /* read fails */
}


static int test_eof(lua_State * L, FILE * f)
{
    int c = getc(f);
    ungetc(c, f);
    lua_pushlstring(L, NULL, 0);
    return (c != EOF);
}

#if 0
static int read_line(lua_State * L, FILE * f)
{
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    for (;;) {
        size_t l;
        char *p = luaL_prepbuffer(&b);
        if (fgets(p, LUAL_BUFFERSIZE, f) == NULL) {     /* eof? */
            luaL_pushresult(&b);        /* close buffer */
            return (lua_strlen(L, -1) > 0);     /* check whether read something */
        }
        l = strlen(p);
        if (l == 0 || p[l - 1] != '\n')
            luaL_addsize(&b, l);
        else {
            luaL_addsize(&b, l - 1);    /* do not include `eol' */
            luaL_pushresult(&b);        /* close buffer */
            return 1;           /* read at least an `eol' */
        }
    }
}
#endif

/* this new version does not care wether the file has 
   line endings using an 'alien' convention */

static int new_read_line(lua_State * L, FILE * f)
{
    luaL_Buffer buf;
    int c, d;
    luaL_buffinit(L, &buf);
    while (1) {
        c = fgetc(f);
        if (c == EOF) {
            luaL_pushresult(&buf);      /* close buffer */
            return (lua_strlen(L, -1) > 0);     /* check whether read something */
        };
        if (c == '\n') {
            break;
        } else if (c == '\r') {
            d = fgetc(f);
            if (d != EOF && d != '\n')
                ungetc(d, f);
            break;
        } else {
            luaL_addchar(&buf, c);
        }
    }
    luaL_pushresult(&buf);      /* close buffer */
    return 1;
}


static int read_chars(lua_State * L, FILE * f, size_t n)
{
    size_t rlen;                /* how much to read */
    size_t nr;                  /* number of chars actually read */
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    rlen = LUAL_BUFFERSIZE;     /* try to read that much each time */
    do {
        char *p = luaL_prepbuffer(&b);
        if (rlen > n)
            rlen = n;           /* cannot read more than asked */
        nr = fread(p, sizeof(char), rlen, f);
        luaL_addsize(&b, nr);
        n -= nr;                /* still have to read `n' chars */
    } while (n > 0 && nr == rlen);      /* until end of count or eof */
    luaL_pushresult(&b);        /* close buffer */
    return (n == 0 || lua_strlen(L, -1) > 0);
}

static int read_all(lua_State * L, FILE * f)
{
    /* attempt to speed up reading whole files */
    long rlen;                  /* how much to read */
    size_t nr;                  /* number of chars actually read */
    long old;                   /* old file location */
    char *p;
    old = ftell(f);
    if (old > 0 && (fseek(f, 0, SEEK_END) >= 0) && ((rlen = ftell(f)) > 0) && rlen < 1000 * 1000 * 100) {       /* 100 MB */
        fseek(f, old, SEEK_SET);
        p = malloc((size_t) rlen);
        if (p != NULL) {
            nr = fread(p, sizeof(char), (size_t) rlen, f);
            lua_pushlstring(L, p, nr);
            free(p);
            return 1;
        }
    }
    if (old > 0)
        fseek(f, old, SEEK_SET);
    return read_chars(L, f, ~((size_t) 0));
}


static int g_read(lua_State * L, FILE * f, int first)
{
    int nargs = lua_gettop(L) - 1;
    int success;
    int n;
    clearerr(f);
    if (nargs == 0) {           /* no arguments? */
        success = new_read_line(L, f);
        n = first + 1;          /* to return 1 result */
    } else {                    /* ensure stack space for all results and for auxlib's buffer */
        luaL_checkstack(L, nargs + LUA_MINSTACK, "too many arguments");
        success = 1;
        for (n = first; nargs-- && success; n++) {
            if (lua_type(L, n) == LUA_TNUMBER) {
                size_t l = (size_t) lua_tointeger(L, n);
                success = (l == 0) ? test_eof(L, f) : read_chars(L, f, l);
            } else {
                const char *p = lua_tostring(L, n);
                luaL_argcheck(L, p && p[0] == '*', n, "invalid option");
                switch (p[1]) {
                case 'n':      /* number */
                    success = read_number(L, f);
                    break;
                case 'l':      /* line */
                    success = new_read_line(L, f);
                    break;
                case 'a':      /* file */
                    read_all(L, f);
                    success = 1;        /* always success */
                    break;
                default:
                    return luaL_argerror(L, n, "invalid format");
                }
            }
        }
    }
    if (ferror(f))
        return pushresult(L, 0, NULL);
    if (!success) {
        lua_pop(L, 1);          /* remove last result */
        lua_pushnil(L);         /* push nil instead */
    }
    return n - first;
}

static int io_read(lua_State * L)
{
    return g_read(L, getiofile(L, IO_INPUT), 1);
}

static int f_read(lua_State * L)
{
    return g_read(L, tofile(L), 2);
}

static int io_readline(lua_State * L)
{
    FILE *f = *(FILE **) lua_touserdata(L, lua_upvalueindex(1));
    int sucess;
    if (f == NULL)              /* file is already closed? */
        luaL_error(L, "file is already closed");
    sucess = new_read_line(L, f);
    if (ferror(f))
        return luaL_error(L, "%s", strerror(errno));
    if (sucess)
        return 1;
    else {                      /* EOF */
        if (lua_toboolean(L, lua_upvalueindex(2))) {    /* generator created file? */
            lua_settop(L, 0);
            lua_pushvalue(L, lua_upvalueindex(1));
            aux_close(L);       /* close it */
        }
        return 0;
    }
}

/* }====================================================== */


static int g_write(lua_State * L, FILE * f, int arg)
{
    int nargs = lua_gettop(L) - 1;
    int status = 1;
    for (; nargs--; arg++) {
        if (lua_type(L, arg) == LUA_TNUMBER) {
            /* optimization: could be done exactly as for strings */
            status = status &&
                fprintf(f, LUA_NUMBER_FMT, lua_tonumber(L, arg)) > 0;
        } else {
            size_t l;
            const char *s = luaL_checklstring(L, arg, &l);
            status = status && (fwrite(s, sizeof(char), l, f) == l);
        }
    }
    return pushresult(L, status, NULL);
}


static int io_write(lua_State * L)
{
    return g_write(L, getiofile(L, IO_OUTPUT), 1);
}


static int f_write(lua_State * L)
{
    return g_write(L, tofile(L), 2);
}


static int f_seek(lua_State * L)
{
    static const int mode[] = { SEEK_SET, SEEK_CUR, SEEK_END };
    static const char *const modenames[] = { "set", "cur", "end", NULL };
    FILE *f = tofile(L);
    int op = luaL_checkoption(L, 2, "cur", modenames);
    long offset = luaL_optlong(L, 3, 0);
    op = fseek(f, offset, mode[op]);
    if (op)
        return pushresult(L, 0, NULL);  /* error */
    else {
        lua_pushinteger(L, ftell(f));
        return 1;
    }
}


static int f_setvbuf(lua_State * L)
{
    int sz, op, res;
    static const int mode[] = { _IONBF, _IOFBF, _IOLBF };
    static const char *const modenames[] = { "no", "full", "line", NULL };
    FILE *f = tofile(L);
    op = luaL_checkoption(L, 2, NULL, modenames);
    lua_number2int(sz, luaL_optinteger(L, 3, LUAL_BUFFERSIZE));
    res = setvbuf(f, NULL, mode[op], (size_t) sz);
    return pushresult(L, res == 0, NULL);
}



static int io_flush(lua_State * L)
{
    return pushresult(L, fflush(getiofile(L, IO_OUTPUT)) == 0, NULL);
}


static int f_flush(lua_State * L)
{
    return pushresult(L, fflush(tofile(L)) == 0, NULL);
}


static const luaL_Reg iolib[] = {
    {"close", io_close},
    {"flush", io_flush},
    {"input", io_input},
    {"lines", io_lines},
    {"open", io_open},
    {"open_ro", io_open_ro},
    {"output", io_output},
    {"popen", io_popen},
    {"read", io_read},
    {"tmpfile", io_tmpfile},
    {"type", io_type},
    {"write", io_write},
    {NULL, NULL}
};


static const luaL_Reg flib[] = {
    {"close", io_close},
    {"flush", f_flush},
    {"lines", f_lines},
    {"read", f_read},
    {"seek", f_seek},
    {"setvbuf", f_setvbuf},
    {"write", f_write},
    {"__gc", io_gc},
    {"__tostring", io_tostring},
    {NULL, NULL}
};


static void createmeta(lua_State * L)
{
    luaL_newmetatable(L, LUA_FILEHANDLE);       /* create metatable for file handles */
    lua_pushvalue(L, -1);       /* push metatable */
    lua_setfield(L, -2, "__index");     /* metatable.__index = metatable */
    luaL_register(L, NULL, flib);       /* file methods */
}


static void createstdfile(lua_State * L, FILE * f, int k, const char *fname)
{
    *newfile(L) = f;
    if (k > 0) {
        lua_pushvalue(L, -1);
        lua_rawseti(L, LUA_ENVIRONINDEX, k);
    }
    lua_setfield(L, -2, fname);
}


LUALIB_API int luaopen_io(lua_State * L)
{
    createmeta(L);
    /* create (private) environment (with fields IO_INPUT, IO_OUTPUT, __close) */
    lua_createtable(L, 2, 1);
    lua_replace(L, LUA_ENVIRONINDEX);
    /* open library */
    luaL_register(L, LUA_IOLIBNAME, iolib);
    /* create (and set) default files */
    createstdfile(L, stdin, IO_INPUT, "stdin");
    createstdfile(L, stdout, IO_OUTPUT, "stdout");
    createstdfile(L, stderr, 0, "stderr");
    /* create environment for 'popen' */
    lua_getfield(L, -1, "popen");
    lua_createtable(L, 0, 1);
    lua_pushcfunction(L, io_pclose);
    lua_setfield(L, -2, "__close");
    lua_setfenv(L, -2);
    lua_pop(L, 1);              /* pop 'popen' */
    /* set default close function */
    lua_pushcfunction(L, io_fclose);
    lua_setfield(L, LUA_ENVIRONINDEX, "__close");
    return 1;
}
