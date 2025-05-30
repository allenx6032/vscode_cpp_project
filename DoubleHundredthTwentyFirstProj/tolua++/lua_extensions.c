
#include "lua_extensions.h"

#if __cplusplus
extern "C" {
#endif

// cjson
#include "lua_cjson.h"

// zlib
#include "lua_zlib.h"

// lpack
#include "lua_pack.h"

// socket
#include "luasocket.h"
#include "socket_mime.h"
#include "socket_scripts.h"

// filesystem
#include "lua_fs.h"

#if CC_SQLITE_ENABLED > 0
// lsqlite3
#include "lua_sqlite3.h"
#endif

static luaL_Reg luax_exts[] = {
    {"cjson", luaopen_cjson_safe},
    {"zlib", luaopen_zlib},
    {"pack", luaopen_pack},
    {"socket.core", luaopen_socket_core},
    {"mime.core", luaopen_mime_core},
    {"lfs", luaopen_lfs},

#if CC_SQLITE_ENABLED > 0
    {"lsqlite3", luaopen_lsqlite3},
#endif

    {NULL, NULL}
};

void luaopen_lua_extensions(lua_State *L)
{
    // load extensions
    luaL_Reg* lib = luax_exts;
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    for (; lib->func; lib++)
    {
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_pop(L, 2);

    // load extensions script
    luaopen_socket_scripts(L);
}

#if __cplusplus
} // extern "C"
#endif
