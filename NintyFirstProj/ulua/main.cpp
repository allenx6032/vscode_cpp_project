#include "xtable.h"
#include "xtype.h"
#include "itable.h"

using namespace std;

lua_State* L;


void init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
}


void dispose()
{
	lua_close(L);
	L = nullptr;
}


void info(const char* path)
{
	string t_path = path;
	t_path += "table.lua.txt";
	luaL_dofile(L, t_path.c_str());
}

int main()
{
	init();
	// luaopen_pb(L);
	const char* search = "lua/";
	const char* table = "table/";
	inner_load(L, search, table, 0);
	info(search);
	system("pause");
	dispose();
	return 0;
}