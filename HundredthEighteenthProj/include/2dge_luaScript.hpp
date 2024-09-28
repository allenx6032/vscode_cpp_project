#ifndef LUA_SCRIPT_HPP_
#define LUA_SCRIPT_HPP_

#include <iostream>
#include <string>
#include <vector>

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

class LuaScript
{
	lua_State* L;
	int level;
public:
	LuaScript(const std::string& filename);
	~LuaScript();
	void printError(const std::string& varName, const std::string& reason);
	int lua_gettostack(const std::string& varName);
	template<typename T>
    T get(const std::string& varName);
	template<typename T>
    T lua_get(const std::string& varName) {
      return 0;
    }
    template<typename T>
    T lua_getdefault() {
      return 0;
    }
    void testprint(const std::string& str);
};
template <typename T>
T LuaScript::get(const std::string& varName) {
	if (!L)
		printError(varName, "Lua error: Script is not loaded");
	T result;
	if (lua_gettostack(varName) == EXIT_SUCCESS)
		result = lua_get<T>(varName);
	else 
		result = lua_getdefault<T>();
	lua_pop(L, level + 1);
	return result;
}
template <>
inline bool LuaScript::lua_get<bool>(const std::string& varName) {
	return (bool)lua_toboolean(L, -1);
}
template <>
inline int LuaScript::lua_get<int>(const std::string& varName) {
	return (int)lua_tonumber(L, -1);
}
template <>
inline float LuaScript::lua_get<float>(const std::string& varName) {
	if (!lua_isnumber(L, -1)) {
		printError(varName, "Lua error: Not a number");
	}
	return (float)lua_tonumber(L, -1);
}
template <>
inline std::string LuaScript::lua_get<std::string>(const std::string& varName) {
	std::string str = "null";
	if (lua_isstring(L, -1))
		str = std::string(lua_tostring(L, -1));
	else 
		printError(varName, "Lua error: Not a string");
	return str;
}
#endif //LUA_SCRIPT_HPP_