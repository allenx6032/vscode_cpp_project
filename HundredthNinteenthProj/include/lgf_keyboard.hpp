#pragma once
#include "lua_includes.h"

namespace Keyboard
{

int keyup(lua_State *L);
int keydown(lua_State *L);
int poll(lua_State *L);

void registerLuaFunctions(lua_State *L);

};
