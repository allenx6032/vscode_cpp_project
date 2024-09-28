#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lua_includes.h"

#define HAS_LUA_GL

#ifdef HAS_LUA_GL
#include "luagl.h"
#include "luaglu.h"
#endif

// #if LUA_VERSION_NUM < 502
// #define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
// #endif
#if (LUA_VERSION_NUM == 501)
#define luaL_setfuncs(L,f,n) luaL_register(L,NULL,f)
#define luaL_newlib(L,f) luaL_register(L,"lpeg",f)
#endif

static int multiplication(lua_State *L) {
  int a = luaL_checkinteger(L, 1);
  int b = luaL_checkinteger(L, 2);
  lua_Integer c = a * b;
  lua_pushinteger(L, c);
  return 1;
}

/* Tarn API call to initialize the game */
static int tarnapi_init(lua_State *LS) {
	lua_pushinteger(LS, 1);
	lua_setglobal(LS, "tarn_initialized");
	return 1;
}

const char *filename = "main.lua";

int main(){
#if LUA_VERSION_NUM > 501
    lua_State* L = luaL_newstate();
#else
    lua_State* L = lua_open();
#endif
    printf("lua_State* L = lua_open()\n");
    luaL_openlibs(L);
    printf("luaL_openlibs(L)\n");
    // register_classes(L);
#ifdef HAS_LUA_GL
	luaopen_luagl(L);
    printf("luaopen_luagl(L)\n");
	luaopen_luaglu(L);
    printf("luaopen_luaglu(L)\n");
#endif
	// lua_newtable(L);
    // printf("lua_newtable(L)\n");
	// const struct luaL_Reg my_functions_registery[] = {{"mul", multiplication}};
    // printf("luaL_Reg my_functions_registery[]\n");
	// luaL_setfuncs(L, my_functions_registery, 0);
    // printf("luaL_setfuncs\n");
	// lua_setglobal(L, "my_functions_registry");
    // printf("lua_setglobal\n");
    // char *code = "print(my_functions_registry.mul(7, 8))";
    // printf("char *code\n");
    // lua_register(L, "tarn_init", tarnapi_init);
    // printf("lua_register\n");


	// if (luaL_loadstring(L, code) == LUA_OK) {
	// 	if (lua_pcall(L, 0, 1, 0) == LUA_OK) {
	// 	  lua_pop(L, lua_gettop(L));
	// 	}
	// }
	// if (luaL_dofile(L, filename))
    printf("luaL_loadfile(L, %s)\n",filename);
	if (luaL_loadfile(L, filename))
    {
        printf("%s\n", lua_tostring(L, -1));
        lua_close(L);
        return 0;
    }
    getchar();
    lua_close(L);


    // FILE *f = fopen(filename, "r");
    // luaL_Buffer b;
    // luaL_buffinit(L, &b);
    // char buf[BUFSIZ];
    // while (fgets(buf, BUFSIZ, f) != NULL)
    // luaL_addlstring(&b, buf, strlen(buf));
    // luaL_pushresult(&b);
    // fclose(f);

    // printf("%s\n", lua_tostring(L, -1));
    // if (lua_gettop(L) > 1) printf("stack size != 1\n");
    // lua_close(L);
    return 0;
}