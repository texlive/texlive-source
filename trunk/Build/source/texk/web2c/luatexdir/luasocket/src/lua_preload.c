
#include <stdlib.h>
#include "lua.h"
#include "lauxlib.h"

#include "headers_lua.c"
#include "mbox_lua.c"
#include "socket_lua.c"
#include "ftp_lua.c"
#include "http_lua.c"
#include "smtp_lua.c"
#include "tp_lua.c"
#include "url_lua.c"
#include "ltn12_lua.c"
#include "mime_lua.c"

#define TEST(A) do { if (A) {						\
    fprintf(stderr,"FATAL error while preloading lua module " #A);	\
    exit(1);								\
	}								\
} while (0)

void
luatex_socketlua_open (lua_State *L) {
    TEST(luatex_mbox_lua_open(L));
    TEST(luatex_headers_lua_open(L));
    TEST(luatex_socket_lua_open(L));
    TEST(luatex_ltn12_lua_open(L));
    TEST(luatex_mime_lua_open(L));
    TEST(luatex_url_lua_open(L));
    TEST(luatex_tp_lua_open(L));
    TEST(luatex_smtp_lua_open(L));
    TEST(luatex_http_lua_open(L));
    TEST(luatex_ftp_lua_open(L));
}
