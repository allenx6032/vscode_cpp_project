#ifndef __API_4_LUA__
#define __API_4_LUA__

extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}


struct TLua_Funcs
{
	const char *name;
	lua_CFunction func;
};

extern TLua_Funcs g_GameFunc[];
extern int g_GetGameFuncSize();

#endif