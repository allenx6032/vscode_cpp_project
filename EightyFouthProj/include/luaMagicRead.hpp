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

#ifndef LUA_MAGIC_READ_HPP
#define LUA_MAGIC_READ_HPP

#include <lua_includes.h>
#include <functional>

/* Read value from Lua */
template<typename T>
T luaMagic_read(lua_State* L, int index)
{
	if(lua_isnoneornil(L, index))
		return nullptr;
	else
		return static_cast<T>(lua_touserdata(L, index));
}

template<>
inline bool luaMagic_read<bool>(lua_State* L, int index)
{
	return lua_toboolean(L, index);
}

template<>
inline char luaMagic_read<char>(lua_State* L, int index)
{
	return static_cast<char>(luaL_checkinteger(L, index));
}


template<>
inline short luaMagic_read<short>(lua_State* L, int index)
{
	return static_cast<short>(luaL_checkinteger(L, index));
}

template<>
inline int luaMagic_read<int>(lua_State* L, int index)
{
	return static_cast<int>(luaL_checkinteger(L, index));
}

template<>
inline long luaMagic_read<long>(lua_State* L, int index)
{
	return static_cast<long>(luaL_checkinteger(L, index));
}

template<>
inline long long luaMagic_read<long long>(lua_State* L, int index)
{
	return static_cast<long long>(luaL_checkinteger(L, index));
}

template<>
inline unsigned char luaMagic_read<unsigned char>(lua_State* L, int index)
{
	return static_cast<unsigned char>(luaL_checkinteger(L, index));
}

template<>
inline unsigned short luaMagic_read<unsigned short>(lua_State* L, int index)
{
	return static_cast<unsigned short>(luaL_checkinteger(L, index));
}

template<>
inline unsigned int luaMagic_read<unsigned int>(lua_State* L, int index)
{
	return static_cast<unsigned int>(luaL_checkinteger(L, index));
}

template<>
inline unsigned long luaMagic_read<unsigned long>(lua_State* L, int index)
{
	return static_cast<unsigned long>(luaL_checkinteger(L, index));
}

template<>
inline unsigned long long luaMagic_read<unsigned long long>(lua_State* L, int index)
{
	return static_cast<unsigned long long>(luaL_checkinteger(L, index));
}

template<>
inline const char* luaMagic_read<const char*>(lua_State* L, int index)
{
	return luaL_checkstring(L, index);
}

template<>
inline float luaMagic_read<float>(lua_State* L, int index)
{
	return static_cast<float>(luaL_checknumber(L, index));
}

template<>
inline double luaMagic_read<double>(lua_State* L, int index)
{
	return static_cast<double>(luaL_checknumber(L, index));
}

#endif