#ifndef QUICK_PLAYER_LUA_CORE_MANUAL_H
#define QUICK_PLAYER_LUA_CORE_MANUAL_H

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

#include "cocos_cocos2d.h"
#include "lua_cocos2dx_LuaScriptHandlerMgr.h"

int luaopen_PlayerLuaCore_Manual(lua_State* tolua_S);

#endif // #ifndef QUICK_PLAYER_LUA_CORE_MANUAL_H
