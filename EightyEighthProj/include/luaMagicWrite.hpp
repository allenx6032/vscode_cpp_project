/******************************************************************************
MIT License

Copyright (c) 2018 Plan C

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef LUA_MAGIC_WRITE_HPP
#define LUA_MAGIC_WRITE_HPP

#include <lua_includes.h>
#include <functional>
#include <string>

/* Write value to Lua */
template<typename T>
void luaMagic_write(lua_State* L, T value)
{
	T* ptr = static_cast<T*>(lua_newuserdata(L, sizeof(L)));
	if(ptr == nullptr)
	{
		lua_pushnil(L);
	}
	T* object = new(ptr) T();
}

template<typename T>
void luaMagic_write(lua_State* L, T* value)
{
	if(value == nullptr)
		lua_pushnil(L);
	else
		lua_pushlightuserdata(L, static_cast<void*>(value));
}

template<>
inline void luaMagic_write(lua_State* L, bool value)
{
	if(value)
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);
}

template<> 
inline void luaMagic_write(lua_State* L, char value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, short value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, int value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, long value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, long long value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, unsigned char value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, unsigned short value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, unsigned int value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, unsigned long value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<> 
inline void luaMagic_write(lua_State* L, unsigned long long value)
{
	lua_pushinteger(L, static_cast<lua_Integer>(value));
}

template<>
inline void luaMagic_write(lua_State* L, float value)
{
	lua_pushnumber(L, static_cast<lua_Number>(value));
}

template<>
inline void luaMagic_write(lua_State* L, double value)
{
	lua_pushnumber(L, static_cast<lua_Number>(value));
}

template<>
inline void luaMagic_write(lua_State* L, const char* value)
{
	lua_pushstring(L, value);
}

template<>
inline void luaMagic_write(lua_State* L, const std::string value)
{
	lua_pushstring(L, value.c_str());
}


#endif