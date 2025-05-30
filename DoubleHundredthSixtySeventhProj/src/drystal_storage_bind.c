/**
 * This file is part of Drystal.
 *
 * Drystal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Drystal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Drystal.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <lua.h>
#ifndef EMSCRIPTEN
#include <stdlib.h>
#endif

#include "drystal_lua_util.h"
#include "drystal_storage_bind.h"
#include "drystal_storage.h"

extern int json_encode(lua_State* L);
extern int json_decode(lua_State* L);

int mlua_store(lua_State* L)
{
	assert(L);

	const char* key = luaL_checkstring(L, 1);

	lua_pushcfunction(L, json_encode);
	lua_pushvalue(L, 2);

	call_lua_function(L, 1, 1); // table in param, returns json

	const char* value = luaL_checkstring(L, -1);

	storage_store(key, value);
	return 0;
}

int mlua_fetch(lua_State* L)
{
	assert(L);

	const char* key = luaL_checkstring(L, 1);
	char* value = storage_fetch(key);

	if (!value || !value[0]) {
#ifndef EMSCRIPTEN
		free(value);
#endif
		lua_pushnil(L);
		return 1;
	}

	lua_pushcfunction(L, json_decode);
	lua_pushstring(L, value);
	call_lua_function(L, 1, 1);

#ifndef EMSCRIPTEN
	free(value);
#endif
	// table is returned by json_decode
	return 1;
}

