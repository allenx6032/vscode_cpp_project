#include "reilua_main.h"
#include "reilua_lua_core.h"
#include "reilua_core.h"

void platformDefineGlobals() {
	lua_State* L = state->luaState;

	lua_getglobal( L, "RL" );
/*DOC_DEFINES_START*/
/*DOC_DEFINES_END*/
	lua_pop( L, -1 );
}

/* Functions. */

/* Events. */

void luaPlatformRegister() {
}
