#include "LEngine_luafunctions.h"
#include "LEngine_logger.h"
#include <cctype>
#include <cstdlib>

#include <SDL.h>

int lua_render_window(lua_State *L) {
  SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderPresent(renderer);
  return 0;
}

int lua_draw_rect(lua_State *L) {
  SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
  int x = luaL_checkinteger(L, 2);
  int y = luaL_checkinteger(L, 3);

  int w = luaL_checkinteger(L, 4);
  int h = luaL_checkinteger(L, 5);

  SDL_Rect rect = {x, y, w, h};

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &rect);

  // lua_pushlightuserdata(L, renderer);
  return 0;
}

int lua_clear_render(lua_State *L) {
  SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
  SDL_RenderClear(renderer);

  return 0;
}

int lua_draw_color_rect(lua_State *L) {
  SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
  int x = luaL_checkinteger(L, 2);
  int y = luaL_checkinteger(L, 3);

  int w = luaL_checkinteger(L, 4);
  int h = luaL_checkinteger(L, 5);

  int r = luaL_checkinteger(L, 6);
  int g = luaL_checkinteger(L, 7);
  int b = luaL_checkinteger(L, 8);
  int a = luaL_checkinteger(L, 9);

  SDL_Rect rect = {x, y, w, h};

  SDL_SetRenderDrawColor(renderer, r, g, b, a);
  SDL_RenderFillRect(renderer, &rect);

  return 0;
}

int lua_exit(lua_State *L) {
  int exit_code = lua_tonumber(L, 1);
  lua_getglobal(L, "exit");
  if (lua_isfunction(L, -1)) {
    lua_pushinteger(L, exit_code);
    lua_pcall(L, 1, 0, 0);
  }
  exit(exit_code);
  return 0;
}

int lua_log(lua_State *L) {
  std::string msg = lua_tostring(L, 1);
  int state = lua_tointeger(L, 2);

  msg_T type;

  switch (state) {
  case 0:
    type = info;
    break;
  case 1:
    type = error;
    break;
  case 2:
    type = warning;
    break;
  }

  Logger logger;
  logger.setBuffer(msg);
  logger.setDebug(false);

  logger.print(type);

  return 0;
}

int lua_log_bold(lua_State *L) {

  std::string msg = lua_tostring(L, 1);
  int state = lua_tointeger(L, 2);

  msg_T type;

  switch (state) {
  case 0:
    type = info;
    break;
  case 1:
    type = error;
    break;
  case 2:
    type = warning;
    break;
  }

  Logger logger;
  logger.setBuffer(msg);
  logger.setDebug(false);

  logger.print(type, true);

  return 2;
}

int lua_clear() {

#ifdef OS_Windows
  system("cls");
#else
  system("clear");
#endif

  return 0;
}

void registerFunctions(lua_State *L) {

  lua_register(L, "exit_game", lua_exit);
  lua_register(L, "log", lua_log);
  lua_register(L, "log_bold", lua_log_bold);
  lua_register(L, "draw_rect", lua_draw_rect);
  lua_register(L, "draw_color_rect", lua_draw_color_rect);
  lua_register(L, "render_window", lua_render_window);
  lua_register(L, "clear_render", lua_clear_render);
}
