#include "2dge_luaScript.hpp"
LuaScript::LuaScript(const std::string& filename) {
	level = 0;
    L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
        std::cout << "Lua error: script not loaded (" << filename << ")\n";
        L = 0;
    }
}
LuaScript::~LuaScript() {
    if (L) 
    	lua_close(L);
}
void LuaScript::printError(const std::string& varName, const std::string& reason)
{
	std::cout << "Lua error: can't get ["<<varName<<"]. " << reason << std::endl;
}
int LuaScript::lua_gettostack(const std::string& varName) {
	level = 0;
	std::string var = "";
	for (unsigned i = 0; i < varName.size(); i++) {
		if (varName.at(i) != '.') {
			var += varName.at(i);
		}
		else {
			if (level == 0)
				lua_getglobal(L, var.c_str());
			else
				lua_getfield(L, -1, var.c_str());
			if (lua_isnil(L, -1)) {
				printError(varName, var + " is not defined");
				return EXIT_FAILURE;
			} else {
				var = "";
				level++;
			}
		}
	}
	if (level == 0)
		lua_getglobal(L, var.c_str());
	else
		lua_getfield(L, -1, var.c_str());
	if (lua_isnil(L, -1)) {
		printError(varName, var + " is not defined");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
void LuaScript::testprint(const std::string& str)
{
	lua_getglobal(L, "testprint");
	lua_pushstring(L, str.c_str());
	lua_call(L, 1, 0);
}