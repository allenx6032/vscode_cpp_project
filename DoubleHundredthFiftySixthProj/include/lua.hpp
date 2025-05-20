// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

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
#ifdef __cplusplus
}
#endif

#endif