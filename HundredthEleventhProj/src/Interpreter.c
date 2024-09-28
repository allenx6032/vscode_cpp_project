/*************************************************
*  LuaGL - an OpenGL binding for Lua
*  2003-2004(c) Fabio Guerra, Cleyde Marlyse
*  www.luagl.sourceforge.net
*-------------------------------------------------
*  Description: This file is a simple example of
*               a Lua interpreter that uses LuaGL.
*-------------------------------------------------
*  Last Update: 14/07/2003
*  Version: v1.0
*-------------------------------------------------
*  See Copyright Notice in LuaGL.h
*************************************************/

#ifdef _WIN32
  #include <windows.h>
#endif

#include <GL/gl.h>
#include <gl/freeglut.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <LuaGL.h>
#include <LuaGLUT.h>

#if LUA_VERSION_NUM < 502
#define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif
// #if (LUA_VERSION_NUM == 501)
// #define luaL_setfuncs(L,f,n) luaL_register(L,NULL,f)
// #define luaL_newlib(L,f) luaL_register(L,"lpeg",f)
// #endif

const char *filename = "test1.lua";

// main function
int main()
{
   //lua_State *L = lua_open();
#if LUA_VERSION_NUM > 501
    lua_State* L = luaL_newstate();
#else
    lua_State* L = lua_open();
#endif


   luaL_openlibs(L);
   // luaopen_base(L);
   // luaopen_table(L);
   // luaopen_io(L);
   // luaopen_string(L);
   // luaopen_math(L);
   luaopen_opengl(L);
   luaopen_glut(L);

   if(luaL_loadfile(L, filename) /*|| lua_pcall(L, 0, 0, 0)*/)
   {
      printf("Error running lua script:\n\n  %s \n\n", lua_tostring(L, -1));
      lua_close(L);
      return 0;
   }

   getchar();
   lua_close(L);

   return 0;
}
