/*
The MIT License

Copyright (c) 2009 - 2014 Liam Devine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
	\file lua_includes.h
	\brief Prevents name mangling and provides a potential location to enable
	compatibility when new Lua versions are released.
	\details No part of OOLua directly includes any Lua header files, instead
	when required they include this header. Contrary to what some people may think,
	this is by design. There is no way to know if a user's version of the Lua
	library was compiled as C++ or C.
*/

#ifndef LUA_INCLUDES_H_
#	define LUA_INCLUDES_H_

//Prevent name mangling
#ifdef __cplusplus
extern "C"
{
#endif
#	include "lua.h"
#	include "lauxlib.h"
#	include "lualib.h"
#	include <stdlib.h>
#	include <unistd.h>
#ifdef __cplusplus
}
#endif

#define CHECK_TYPE(L, i, t) (lua_type(L, i) == t)
#define USERDATA_POINTER(L, i, type) (*(type **)lua_touserdata(L, i))

// #if LUA_VERSION_NUM < 502
// #define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
// #endif
#if (LUA_VERSION_NUM == 501)
#define luaL_setfuncs(L,f,n) luaL_register(L,NULL,f)
#define luaL_newlib(L,f) luaL_register(L,"lpeg",f)
#endif
#if LUA_VERSION_NUM >= 503
    #define lua_tointegerx lua_tointegerx
#else
    #define lua_tointegerx(L,idx,isnum) \
        (*(isnum) = lua_isnumber(L,idx), lua_tointeger(L,idx))
#endif

#endif //LUA_INCLUDES_H_
