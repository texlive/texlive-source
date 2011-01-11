/*
 *  KANJI Code conversion routines.
 */

#include <kpathsea/config.h>
#include <kpathsea/c-memstr.h>
#include <kpathsea/variable.h>
#include <kpathsea/readable.h>
#include <kpathsea/c-limits.h>

#include <ptexenc/c-auto.h>
#include <ptexenc/ptexenc.h>
#include <ptexenc/kanjicnv.h>
#include <ptexenc/unicode.h>
#include <ptexenc/unicode-jp.h>

#include <ctype.h>

#define ENC_UNKNOWN  0
#define ENC_JIS      1
#define ENC_EUC      2
#define ENC_SJIS     3
#define ENC_UTF8     4

#define ESC '\033'

#ifndef NOFILE
# ifndef OPEN_MAX
#  define OPEN_MAX 24 /* The POSIX minimum. */
# endif
# define NOFILE OPEN_MAX
#endif

const char *ptexenc_version_string = "ptexenc " PTEXENC_VERSION;

static int     file_enc = ENC_UNKNOWN;
static int internal_enc = ENC_UNKNOWN;
static int terminal_enc = ENC_UNKNOWN;


static const_string enc_to_string(int enc)
{
    switch (enc) {
    case ENC_JIS:  return "jis";
    case ENC_EUC:  return "euc";
    case ENC_SJIS: return "sjis";
    case ENC_UTF8: return "utf8";
    default:       return "?";
    }
}

static int string_to_enc(const_string str)
{
    if (str == NULL)                    return ENC_UNKNOWN;
    if (strcasecmp(str, "default")== 0) return DEFAULT_KANJI_ENC;
    if (strcasecmp(str, "jis")    == 0) return ENC_JIS;
    if (strcasecmp(str, "euc")    == 0) return ENC_EUC;
    if (strcasecmp(str, "sjis")   == 0) return ENC_SJIS;
    if (strcasecmp(str, "utf8")   == 0) return ENC_UTF8;
    return -1; /* error */
}

static int get_default_enc(void)
{
    /* kpse_var_value("PTEX_KANJI_ENC") aborts
       if 'kpse_program_name' is empty.  It typically occurs
       when 'ptex' and 'jmpost' print version messages. */
    string var = getenv("PTEX_KANJI_ENC");
    int enc = string_to_enc(var);
    if (enc < 0) {
        fprintf(stderr, "Warning: Unknown environment value "
                "PTEX_KANJI_ENC='%s'\n", var);
    } else if (enc != ENC_UNKNOWN) {
        return enc;
    }
    return DEFAULT_KANJI_ENC;
}

static void set_file_enc(int enc)
{
    file_enc = enc;
}

static void set_internal_enc(int enc)
{
    if      (enc == ENC_SJIS)  internal_enc = ENC_SJIS;
    else /* EUC, JIS, UTF8 */  internal_enc = ENC_EUC;
}

static int get_file_enc(void)
{
    if (file_enc == ENC_UNKNOWN) set_file_enc(get_default_enc());
    return file_enc;
}

static int get_internal_enc(void)
{
    if (internal_enc == ENC_UNKNOWN) set_internal_enc(get_default_enc());
    return internal_enc;
}

static int get_terminal_enc(void)
{
    if (terminal_enc == ENC_UNKNOWN) {
        char lang[16];  /* enough large space */
        const char *s    = getenv("LC_ALL");
        if (s == NULL) s = getenv("LC_MESSAGES");
        if (s == NULL) s = getenv("LANG");
        if (s == NULL) s = getenv("LANGUAGE");
        if (s == NULL) s = "";
        if (strrchr(s, '.') != NULL) s = strrchr(s, '.') + 1;
        strncpy(lang, s, sizeof(lang) - 1);
        lang[sizeof(lang) - 1] = '\0';
        if      (strcasecmp(lang, "euc")  == 0) terminal_enc = ENC_EUC;
        else if (strcasecmp(lang, "eucJP")== 0) terminal_enc = ENC_EUC;
        else if (strcasecmp(lang, "ujis") == 0) terminal_enc = ENC_EUC;
        else if (strcasecmp(lang, "sjis") == 0) terminal_enc = ENC_SJIS;
        else if (strcasecmp(lang, "utf8") == 0) terminal_enc = ENC_UTF8;
        else if (strcasecmp(lang, "UTF-8")== 0) terminal_enc = ENC_UTF8;
        else if (strcasecmp(lang, "jis")  == 0) terminal_enc = ENC_JIS;
        else if (strcasecmp(lang, "ISO-2022-JP")== 0) terminal_enc = ENC_JIS;
        else terminal_enc = get_file_enc();
    }
    return terminal_enc;
}

const_string get_enc_string(void)
{
    static char buffer[20]; /* enough large space */

    if (get_file_enc() == get_internal_enc()) {
        return enc_to_string(get_file_enc());
    } else {
        sprintf(buffer, "%s.%s",
                enc_to_string(get_file_enc()),
                enc_to_string(get_internal_enc()));
        return buffer;
    }
}

boolean set_enc_string(const_string file_str, const_string internal_str)
{
    int file     = string_to_enc(file_str);
    int internal = string_to_enc(internal_str);

    if (file < 0 || internal < 0) return false; /* error */
    if (file     != ENC_UNKNOWN) {  set_file_enc(file);  nkf_disable();  }
    if (internal != ENC_UNKNOWN) set_internal_enc(internal);
    return true;
}

boolean is_internalSJIS(void)
{
    return (internal_enc == ENC_SJIS);
}


/* check char range (kanji 1st) */
boolean iskanji1(int c)
{
    if (is_internalSJIS()) return isSJISkanji1(c);
    /* EUC */              return isEUCkanji1(c);
}

/* check char range (kanji 2nd) */
boolean iskanji2(int c)
{
    if (is_internalSJIS()) return isSJISkanji2(c);
    /* EUC */              return isEUCkanji2(c);
}

/* multi-byte char length in s[pos] */
int multistrlen(string s, int len, int pos)
{
    s += pos; len -= pos;
    if (len < 2) return 1;
    if (is_internalSJIS()) {
        if (isSJISkanji1(s[0]) && isSJISkanji2(s[1])) return 2;
    } else { /* EUC */
        if (isEUCkanji1(s[0])  && isEUCkanji2(s[1]))  return 2;
    }
    return 1;
}

/* buffer (EUC/SJIS/UTF-8) to internal (EUC/SJIS) code conversion */
long fromBUFF(string s, int len, int pos)
{
    s += pos; len -= pos;
    if (len < 2) return s[0];
    if (is_internalSJIS()) {
        if (isSJISkanji1(s[0]) && isSJISkanji2(s[1])) return HILO(s[0], s[1]);
    } else { /* EUC */
        if (isEUCkanji1(s[0])  && isEUCkanji2(s[1]))  return HILO(s[0], s[1]);
    }
    return s[0];
}

/* internal (EUC/SJIS) to buffer (EUC/SJIS) code conversion */
long toBUFF(long kcode)
{
    return kcode;
}


/* JIS to internal (EUC/SJIS) code conversion */
long fromJIS(long kcode)
{
    if (is_internalSJIS())  return JIStoSJIS(kcode);
    /* EUC */               return JIStoEUC(kcode);
}

/* internal (EUC/SJIS) to JIS code conversion */
long toJIS(long kcode)
{
    if (is_internalSJIS())  return SJIStoJIS(kcode);
    /* EUC */               return EUCtoJIS(kcode);
}


/* EUC to internal (EUC/SJIS) code conversion */
long fromEUC(long kcode)
{
    if (is_internalSJIS()) return fromJIS(EUCtoJIS(kcode));
    /* EUC */              return kcode;
}

/* internal (EUC/SJIS) to EUC code conversion */
static long toEUC(long kcode)
{
    if (is_internalSJIS()) return JIStoEUC(toJIS(kcode));
    /* EUC */              return kcode;
    
}


/* SJIS to internal (EUC/SJIS) code conversion */
long fromSJIS(long kcode)
{
    if (is_internalSJIS()) return kcode;
    /* EUC */              return fromJIS(SJIStoJIS(kcode));
}

/* internal (EUC/SJIS) to SJIS code conversion */
static long toSJIS(long kcode)
{
    if (is_internalSJIS()) return kcode;
    /* EUC */              return JIStoSJIS(toJIS(kcode));
}


/* KUTEN to internal (EUC/SJIS) code conversion */
long fromKUTEN(long kcode)
{
    return fromJIS(KUTENtoJIS(kcode));
}


/* UCS to internal (EUC/SJIS) code conversion */
static long fromUCS(long kcode)
{
    kcode = UCS2toJIS(kcode);
    if (kcode == 0) return 0;
    return fromJIS(kcode);
}

/* internal (EUC/SJIS) to UCS code conversion */
long toUCS(long kcode)
{
    return JIStoUCS2(toJIS(kcode));
}

/* internal (EUC/SJIS) to 'enc' code conversion */
static long toENC(long kcode, int enc)
{
    switch (enc) {
    case ENC_UTF8: return UCStoUTF8(toUCS(kcode));
    case ENC_JIS:  return toJIS(kcode);
    case ENC_EUC:  return toEUC(kcode);
    case ENC_SJIS: return toSJIS(kcode);
    default:
        fprintf(stderr, "toENC: unknown enc (%d).\n", enc);
        return 0;
    }
}



#define KANJI_IN   LONG(0, ESC, '$', 'B')
#define KANJI_OUT  LONG(0, ESC, '(', 'B')

static int put_multibyte(long c, FILE *fp) {
    if (BYTE1(c) != 0 && putc(BYTE1(c), fp) == EOF) return EOF;
    if (BYTE2(c) != 0 && putc(BYTE2(c), fp) == EOF) return EOF;
    if (BYTE3(c) != 0 && putc(BYTE3(c), fp) == EOF) return EOF;
    /* always */  return putc(BYTE4(c), fp);
}

/* putc() with code conversion */
int putc2(int c, FILE *fp)
{
    static int inkanji[NOFILE]; /* 0: not in Kanji
                                   1: in JIS Kanji and first byte is in c1[]
                                  -1: in JIS Kanji and c1[] is empty */
    static unsigned char c1[NOFILE];
    const int fd = fileno(fp);
    int ret = c, output_enc;

    if (fp == stdout || fp == stderr) output_enc = get_terminal_enc();
    else                              output_enc = get_file_enc();

    if (inkanji[fd] > 0) {   /* KANJI 2nd */
        ret = put_multibyte(toENC(HILO(c1[fd], c), output_enc), fp);
        inkanji[fd] = -1;
    } else if (iskanji1(c)) { /* KANJI 1st */
        if (inkanji[fd] == 0 && output_enc == ENC_JIS) {
            ret = put_multibyte(KANJI_IN, fp);
        }
        c1[fd] = c;
        inkanji[fd] = 1;
    } else {                  /* ASCII */
        if (inkanji[fd] < 0 && output_enc == ENC_JIS) {
            ret = put_multibyte(KANJI_OUT, fp);
        }
        ret = putc(c, fp);
        inkanji[fd] = 0;
    }
    return ret;
}

/* fputs() with code conversion */
int fputs2(const char *s, FILE *fp)
{
    while (*s != '\0') {
        int ret = putc2((unsigned char)*s, fp);
        if (ret == EOF) return EOF;
        s++;
    }
    return 1;
}


static struct unget_st {
    int size;
    int buff[4];
} ungetbuff[NOFILE];

static int getc4(FILE *fp)
{
    struct unget_st *p = &ungetbuff[fileno(fp)];

    if (p->size == 0) return getc(fp);
    return p->buff[--p->size];
}

static int ungetc4(int c, FILE *fp)
{
    struct unget_st *p = &ungetbuff[fileno(fp)];

    if (p->size >= 4) return EOF;
    return p->buff[p->size++] = c;
}


static string buffer;
static long first, last;
static boolean combin_voiced_sound(boolean semi)
{
    int i;

    if (last-2 < first) return false;
    if (multistrlen(buffer,last,last-2) != 2) return false;
    i = toUCS(fromBUFF(buffer,last,last-2));
    i = get_voiced_sound(i, semi);
    if (i == 0) return false;
    i = toBUFF(fromUCS(i));
    buffer[last-2] = HI(i);
    buffer[last-1] = LO(i);
    return true;
}

static void write_multibyte(long i)
{
    if (BYTE1(i) != 0) buffer[last++] = BYTE1(i);
    if (BYTE2(i) != 0) buffer[last++] = BYTE2(i);
    /* always */       buffer[last++] = BYTE3(i);
    /* always */       buffer[last++] = BYTE4(i);
}

static void write_hex(int i)
{
    sprintf(buffer + last, "^^%02x", i);
    last += 4;
}

/* getc() with check of broken encoding of UTF-8 */
static int getcUTF8(FILE *fp)
{
    int c = getc4(fp);

    if (isUTF8(2,2,c)) return c;
    ungetc4(c, fp);
    return EOF;
}

static void get_utf8(int i, FILE *fp)
{
    long u = 0, j;
    int i2 = EOF, i3 = EOF, i4 = EOF;

    switch (UTF8length(i)) {
    case 2:
        i2 = getcUTF8(fp); if (i2 == EOF) break;
        u = UTF8BtoUCS(i, i2);
        break;
    case 3:
        i2 = getcUTF8(fp); if (i2 == EOF) break;
        i3 = getcUTF8(fp); if (i3 == EOF) break;
        u = UTF8CtoUCS(i, i2, i3);
        if (u == U_BOM) return; /* just ignore */
        if (u == U_VOICED      && combin_voiced_sound(false)) return;
        if (u == U_SEMI_VOICED && combin_voiced_sound(true))  return;
        break;
    case 4:
        i2 = getcUTF8(fp); if (i2 == EOF) break;
        i3 = getcUTF8(fp); if (i3 == EOF) break;
        i4 = getcUTF8(fp); if (i4 == EOF) break;
        u = UTF8DtoUCS(i, i2, i3, i4);
        break;
    default:
        u = U_REPLACEMENT_CHARACTER;
        break;
    }

    j = toBUFF(fromUCS(u));
    if (j == 0) { /* can't represent (typically umlaut o in EUC) */
        write_hex(i);
        if (i2 != EOF) write_hex(i2);
        if (i3 != EOF) write_hex(i3);
        if (i4 != EOF) write_hex(i4);
    } else {
        write_multibyte(j);
    }
}

static void get_euc(int i, FILE *fp)
{
    int j = getc4(fp);

    if (isEUCkanji2(j)) {
        write_multibyte(toBUFF(fromEUC(HILO(i,j))));
    } else {
        buffer[last++] = i;
        ungetc4(j, fp);
    }
}        

static void get_sjis(int i, FILE *fp)
{
    int j = getc4(fp);

    if (isSJISkanji2(j)) {
        write_multibyte(toBUFF(fromSJIS(HILO(i,j))));
    } else {
        buffer[last++] = i;
        ungetc4(j, fp);
    }
}        

static boolean is_tail(long *c, FILE *fp)
{
    if (*c == EOF) return true;
    if (*c == '\n') return true;
    if (*c == '\r') {
        int d = getc4(fp);
        if (d == '\n') *c = d;
        else ungetc4(d, fp);
        return true;
    }
    return false;
}

#define MARK_LEN 4
/* if stream begins with BOM + 7bit char */
static boolean isUTF8Nstream(FILE *fp)
{
    int i;
    int c[MARK_LEN];
    int bom_u[MARK_LEN] = { 0xEF, 0xBB, 0xBF, 0x7E };
    int bom_l[MARK_LEN] = { 0xEF, 0xBB, 0xBF, 0 };

    for (i=0; i<MARK_LEN; i++) {
        c[i] = getc4(fp);
        if (!(bom_l[i] <= c[i] && c[i] <= bom_u[i])) {
            do { ungetc4(c[i], fp); } while (--i>0);
            return false;
        }
    }
    ungetc4(c[MARK_LEN-1], fp);
    return true;
}

static int infile_enc[NOFILE]; /* ENC_UNKNOWN (=0): not determined
                                  other: determined */

/* input line with encoding conversion */
long input_line2(FILE *fp, string buff, long pos,
		 const long buffsize, int *lastchar)
{
    long i;
    static boolean injis = false;
    const int fd = fileno(fp);

    if (infile_enc[fd] == ENC_UNKNOWN) { /* just after opened */
        if (isUTF8Nstream(fp)) infile_enc[fd] = ENC_UTF8;
        else                   infile_enc[fd] = get_file_enc();
    }
    buffer = buff;
    first = last = pos;

    while (last < buffsize-30 && (i=getc4(fp)) != EOF && i!='\n' && i!='\r') {
        /* 30 is enough large size for one char */
        /* attention: 4 times of write_hex() eats 16byte */
        if (i == ESC) {
            if ((i=getc4(fp)) == '$') { /* ESC '$' (Kanji-in) */
                i = getc4(fp);
                if (i == '@' || i == 'B') {
                    injis = true;
                } else {               /* broken Kanji-in */
                    buffer[last++] = ESC;
                    buffer[last++] = '$';
                    if (is_tail(&i, fp)) break;
                    buffer[last++] = i;
                }
            } else if (i == '(') {     /* ESC '(' (Kanji-out) */
                i = getc4(fp);
                if (i == 'J' || i == 'B' || i == 'H') {
                    injis = false;
                } else {               /* broken Kanji-out */
                    buffer[last++] = ESC;
                    buffer[last++] = '(';
                    if (is_tail(&i, fp)) break;
                    buffer[last++] = i;
                }
            } else { /* broken ESC */
                buffer[last++] = ESC;
                if (is_tail(&i, fp)) break;
                buffer[last++] = i;
            }
        } else { /* rather than ESC */
            if (injis) { /* in JIS */
                long j = getc4(fp);
                if (is_tail(&j, fp)) {
                    buffer[last++] = i;
                    i = j;
                    break;
                } else { /* JIS encoding */
                    i = fromJIS(HILO(i,j));
                    if (i == 0) i = fromUCS(U_REPLACEMENT_CHARACTER);
                    write_multibyte(toBUFF(i));
                }
            } else {  /* normal */
                if        (infile_enc[fd] == ENC_SJIS && isSJISkanji1(i)) {
                    get_sjis(i, fp);
                } else if (infile_enc[fd] == ENC_EUC  && isEUCkanji1(i)) {
                    get_euc(i, fp);
                } else if (infile_enc[fd] == ENC_UTF8 && UTF8length(i) > 1) {
                    get_utf8(i, fp);
                } else {
                    buffer[last++] = i;
                }
            }
        }
    }

    buffer[last] = '\0';
    if (i == EOF || i == '\n' || i == '\r') injis = false;
    if (lastchar != NULL) *lastchar = i;
    return last;
}


static const_string in_filter = NULL;
static FILE *piped_fp[NOFILE];
static int piped_num = 0;

void nkf_disable(void)
{
    in_filter = "";
}

#ifdef NKF_TEST
#include <stdlib.h>
static void nkf_check(void)
{
    if (piped_num > 0) {
        fprintf(stderr, "nkf_check: %d nkf_open() did not closed.\n",
                piped_num);
    } else {
        fprintf(stderr, "nkf_check: nkf_open() OK.\n");
    }
}
#endif /* NKF_TEST */

/* 'mode' must be read */
FILE *nkf_open(const char *path, const char *mode) {
    char buff[PATH_MAX * 2 + 20];  /* 20 is enough gaps */
    FILE *fp;

    if (in_filter == NULL) {
        in_filter = kpse_var_value("PTEX_IN_FILTER");
        if (in_filter == NULL || strcasecmp(in_filter, "no") == 0) {
            nkf_disable();
        }
#ifdef NKF_TEST
        atexit(nkf_check);
#endif /* NKF_TEST */
    }

    if (in_filter[0] == '\0') return fopen(path, mode);
    path = kpse_readable_file(path);
    if (path == NULL) return NULL; /* can't read */

    sprintf(buff, "%.*s < '%.*s'", PATH_MAX, in_filter, PATH_MAX, path);
    /* fprintf(stderr, "\n`%s`", buff); */
    fp = popen(buff , "r");
    if (piped_num < NOFILE) piped_fp[piped_num++] = fp;
    return fp;
}

/* we must close in stack order (FILO) or in queue order (FIFO) */
int nkf_close(FILE *fp) {
    infile_enc[fileno(fp)] = ENC_UNKNOWN;
    if (piped_num > 0) {
        if (fp == piped_fp[piped_num-1]) {  /* for FILO */
            piped_num--;
            return pclose(fp);
        }
        if (fp == piped_fp[0]) {  /* for FIFO */
            int i;
            piped_num--;
            for (i=0; i<piped_num; i++) piped_fp[i] = piped_fp[i+1];
            return pclose(fp);
        }
    }
    return fclose(fp);
}
