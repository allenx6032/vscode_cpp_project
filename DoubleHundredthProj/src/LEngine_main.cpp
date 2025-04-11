#include <chrono>
#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <memory>
#include <string>
#include <thread>

#include "LEngine_game.h"
#include "LEngine_luafunctions.h"

#ifndef OS_Windows
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#else
#include <fcntl.h>
#include <windows.h>
#include <unistd.h>
#endif

#ifndef OS_Windows
char getChar() {
  struct termios oldt, newt;
  char ch = '\0';

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

  ssize_t bytesRead = read(STDIN_FILENO, &ch, 1);
  if (bytesRead <= 0) {
    ch = '\0'; // No character read
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  fcntl(STDIN_FILENO, F_SETFL, flags);

  return ch;
}

#else

void setConsoleMode(bool enable) {
  HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(hConsole, &mode);

  if (enable) {
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  } else {
    mode |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
  }

  SetConsoleMode(hConsole, mode);
}

char getChar() {
  HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(hConsole, &mode);

  setConsoleMode(true);

  INPUT_RECORD inputRecord;
  DWORD eventsRead;
  char ch = '\0';

  if (ReadConsoleInput(hConsole, &inputRecord, 1, &eventsRead)) {
    #ifndef OS_Windows
    if (inputRecord.EventType == KEY_EVENT &&
        inputRecord.Event.EventType.KeyEvent.bKeyDown) {
      ch = inputRecord.Event.EventType.KeyEvent.uChar.AsciiChar;
    }
    #else
    if (inputRecord.EventType == KEY_EVENT &&
        inputRecord.Event.KeyEvent.bKeyDown) {
      ch = inputRecord.Event.KeyEvent.uChar.AsciiChar;
    }
    #endif
  }

  setConsoleMode(false);

  return ch;
}

#endif

bool running = true;

int main(int argc, char **argv) {
  int status_code = 0;
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  registerFunctions(L);

  enum file_enum file_t = none;

  std::unique_ptr<config_t> configurations(new configs);

  if (FileExists("config.lua") && Check(L, luaL_dofile(L, "config.lua")) &&
      argc < 2) {
    lua_getglobal(L, "conf");
    if (lua_istable(L, -1)) {
      lua_pushstring(L, "name");
      lua_gettable(L, -2);
      if (lua_isstring(L, -1))
        configurations->name = lua_tostring(L, -1);
      lua_pop(L, 1);

      lua_pushstring(L, "fps");
      lua_gettable(L, -2);
      if (lua_isnumber(L, -1))
        configurations->fps = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_pushstring(L, "width");
      lua_gettable(L, -2);
      if (lua_isnumber(L, -1))
        configurations->width = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_pushstring(L, "height");
      lua_gettable(L, -2);
      if (lua_isnumber(L, -1))
        configurations->height = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_pushstring(L, "terminal");
      lua_gettable(L, -2);
      if (lua_isboolean(L, -1))
        configurations->terminal = lua_toboolean(L, -1);
      lua_pop(L, 1);
    }

    lua_pop(L, 1);
  } else if (!FileExists("config.lua") && argc > 1) {
    if (std::strncmp(argv[1], "conf", 3) == 0) {
      file_t = config;
      init_file(file_t);
      return 0;
    }
  }

  // if(strlen(configurations->name.c_str()) == 0)
  //   configurations->name = "Game";

  if (!FileExists("init.lua")) {
    file_t = init;
    init_file(file_t);
  }

  if (Check(L, luaL_dofile(L, "init.lua"))) {
    lua_getglobal(L, "start");

    if (lua_isfunction(L, -1)) {
      lua_pushstring(L, configurations->name.c_str());
      if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        return lua_tonumber(L, -2);
      }
      if (lua_isnumber(L, -1) && lua_tonumber(L, -1) > 0) {
        std::cout << "Error: start function returned error code\n code -> "
                  << lua_tonumber(L, -1) << std::endl;
        return lua_tonumber(L, -1);
      }
      /*if(Check(L, lua_pcall(L, 1, 1, 0))) {
        on_el_exit(L, lua_tointeger(L, -1));
      }*/
    }

    std::thread update(
        [&configurations](lua_State *L, int *status_code, bool *running) {
          lua_getglobal(L, "update");
          if (lua_isfunction(L, -1)) {
            auto previousTime = std::chrono::high_resolution_clock::now();

            while (running) {
              lua_getglobal(L, "run");
              if (lua_isboolean(L, -1))
                *running = lua_toboolean(L, -1);
              lua_pop(L, 1);

              std::string str(1, getChar());
              lua_pushstring(L, str.c_str());
              lua_setglobal(L, "pressed_key");

              lua_getglobal(L, "update");
              auto currentTime = std::chrono::high_resolution_clock::now();
              std::chrono::duration<float> elapsedTime =
                  currentTime - previousTime;
              float deltaTime = elapsedTime.count();

              lua_pushnumber(L, deltaTime);
              if (!Check(L, lua_pcall(L, 1, 0, 0)))
                break;

              previousTime = currentTime;

              const float frameDuration = 1.0f / configurations->fps;

              std::chrono::duration<float> frameTime =
                  std::chrono::high_resolution_clock::now() - currentTime;
              if (frameTime.count() < frameDuration) {
                std::this_thread::sleep_for(std::chrono::duration<float>(
                    frameDuration - frameTime.count()));
              }
            }

            if (*running == false) {
              lua_getglobal(L, "status_code");
              if (lua_isnumber(L, -1))
                *status_code = lua_tonumber(L, -1);
              // std::cout << "EXITED WITH STATUS CODE: " << *status_code <<
              // std::endl;
              lua_pop(L, 1);

              lua_getglobal(L, "exit");
              if (lua_isfunction(L, -1)) {
                lua_pushnumber(L, 1);
                if (Check(L, lua_pcall(L, 1, 0, 0))) {
                }
              }
            }

            lua_pop(L, 1);
          }
        },
        L, &status_code, &running);

    if (!configurations->terminal) {

      std::thread win([&configurations, &L]() {
        init_window(configurations->width, configurations->height, L);
      });

      try {
        win.join();
      } catch (Error) {
      }
    }

    update.join();
  }

  lua_close(L);
  return 0;
}
