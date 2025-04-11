#ifndef LUA_FUNCTIONS_H
#define LUA_FUNCTIONS_H

#include <lua.hpp>

void registerFunctions(lua_State *L);
int lua_render_window(lua_State *L);
int lua_clear_render(lua_State *L);
int lua_draw_rect(lua_State *L);
int lua_draw_color_rect(lua_State *L);

int lua_exit(lua_State *L);
// int lua_key_pressed(lua_State *L);
int lua_log(lua_State *L);
int lua_log_bold(lua_State *L);

#endif
