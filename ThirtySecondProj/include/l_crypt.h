#include <lua.h>
#include <lauxlib.h>

int lsha1(lua_State *L);
int lhmac_sha1(lua_State *L);



#ifndef __LUA_CRYPT_H__
#define __LUA_CRYPT_H__

#ifdef __cplusplus
extern "C" {
#endif

int luaopen_crypt(lua_State *L);

#ifdef __cplusplus
}
#endif
#endif