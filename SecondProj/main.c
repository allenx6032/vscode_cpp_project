#include <stdlib.h>
#include <stdio.h>
#include "lautoc.h"
#ifndef LUAA_LUAIMPLEMENTATION
#define LUAA_LUAIMPLEMENTATION
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif


float power(float val, int pow) {
  float x = 1.0;
  for(int i = 0; i < pow; i++) {
    x = x * val;
  }
  return x;
}

int main()
{
  lua_State *L = luaL_newstate();  /* create state */
  if (L == NULL) {
    printf( "cannot create state: not enough memory");
    return EXIT_FAILURE;
  }
  lua_close(L);

  L = luaL_newstate();
	luaA_open(L);
	luaA_function(L, power, float, float, int);
	lua_pushnumber(L, 4.2);
	lua_pushinteger(L, 3);
	luaA_call(L, power);
	printf("Result: %f\n", lua_tonumber(L, -1));
	lua_pop(L, 1);
	luaA_close(L);
	lua_close(L);

  printf(  "Hello world!");
}
