#include <iostream>
#include <string>
#include <vector>

#include "lua_includes.h"
#include <SDL.h>

#include "lgf_window.hpp"
#include "lgf_renderer.hpp"
#include "lgf_rectangle.hpp"
#include "lgf_mouse.hpp"
#include "lgf_keyboard.hpp"
#include "lgf_image.hpp"
#include "lgf_font.hpp"

SDL_Window *win;
SDL_Renderer *ren;

const char *filename = "lgf_main.lua";

int main(int argc, char *argv[]) {
  // Make sure a Lua script argument has been given
  // if (argc == 1) {
  //   std::cout << "Give a Lua script's file location!" << std::endl;
  //   exit(1);
  // }

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  Window::registerLuaFunctions(L);
  Renderer::registerLuaFunctions(L);
  Rectangle::registerLuaFunctions(L);
  Mouse::registerLuaFunctions(L);
  Keyboard::registerLuaFunctions(L);
  ImageLoader::registerLuaFunctions(L);
  FontLoader::registerLuaFunctions(L);

  luaL_dofile(L, filename);
  std::cout << lua_tostring(L, -1) << std::endl;
  lua_close(L);

  return 0;
}
