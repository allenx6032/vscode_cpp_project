#ifndef GAME_H
#define GAME_H

#include "LEngine_luafunctions.h"
// #include <cstdint>
// #include <cstring>
#include <iostream>
#include <lua.hpp>
#include <string>
#include <stdint.h>
#include <string.h>

#include <SDL.h>
#include <thread>

#define uint unsigned int

void exit(lua_State *);
void init_window(int, int, lua_State *);

#define PREP_ERROR(n, t, m)                                                    \
  {                                                                            \
    (n).type = (t);                                                            \
    (n).errormsg = (m);                                                        \
  }

typedef struct configs {
  std::string name = "Game";
  uint16_t fps = 60;
  bool terminal = true;
  int width = 640;
  int height = 480;
} config_t;

enum file_enum : uint {
  init,
  game,
  config,
  none = 0,
};

enum Error {
  NOT_IMPLEMENTED,
  SYNTAX_ERROR,
  NOT_ENOUGH_ARGUMENTS,
};

typedef struct ERROR_STRUCT {
  char *errormsg;
  enum Error type;
} error;

void print_error(error);

bool Check(lua_State *, int);
bool DirExist(const char *);
bool FileExists(const char *);

void init_gamefile();
void init_file(file_enum);

void tolowerstr(char *str);
void toupperstr(char *str);
#endif
