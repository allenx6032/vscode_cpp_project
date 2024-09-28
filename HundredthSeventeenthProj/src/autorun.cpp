#include <lua_includes.h>
#include <autorun.h>
#include <windows.h>
#include <cstdio>
int main(int argc,char* argv[])
{
	lua_State *L = luaL_newstate();
	toregister(L);/* to register ege functions */
	luaL_openlibs(L);
	if(argc == 1? luaL_dofile(L,"./main.lua") : luaL_dofile(L,argv[1]))
	{
		MessageBox(NULL,"Error","EGE for Lua",MB_OK);
	}
	lua_close(L);
	return 0;
}