#pragma once
// #include "lua.h"
// #include "lualib.h"
// #include "lauxlib.h"
#include <SDL.h>
#include <sol.hpp>
#undef main
#include <SDL_image.h>
#include <SDL_mixer2.h>
#include <SDL_ttf.h>
#include <memory>

void InitEngine();
void InitLuaEngine(lua_State* L);
lua_State* CreateLuaEngine();
void CloseEngine();
