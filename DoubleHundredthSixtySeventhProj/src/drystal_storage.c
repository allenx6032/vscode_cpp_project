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

#include "drystal_storage.h"
#include "drystal_util.h"

#ifdef EMSCRIPTEN
#include <emscripten.h>

char *storage_fetch(const char *key)
{
	assert(key);

	char *js;
	js = strjoin("if (localStorage!==undefined) {localStorage['", key, "']||''} else {''}", NULL);

	char *value = emscripten_run_script_string(js);
	return value;
}

void storage_store(const char *key, const char *value)
{
	assert(key);
	assert(value);

	char *js;

	js = strjoin("localStorage['", key, "'] = '", value, "';", NULL);

	emscripten_run_script(js);
}

#else
#include <stdio.h>
#include <lua.h>

#ifdef _WIN32
    // Windows 代码
    #include "mman.h"
#else
    // POSIX 代码，如 Linux
    #include <sys/mman.h>
    #include <unistd.h>
#endif

#include "drystal_lua_util.h"
#include "drystal_macro.h"
#include "drystal_log.h"

extern int json_encode(lua_State * L);
extern int json_decode(lua_State * L);

char *storage_fetch(const char *key)
{
	long filesize = 0;
	FILE *file = NULL;
	char *data = NULL;
	char *value;
	lua_State *L;

	assert(key);

	L = luaL_newstate();
	if (!L)
		log_oom_and_exit();

	file = fopen(".storage", "r");
	if (!file) {
		goto fail;
	}

	fseek(file, 0L, SEEK_END);
	filesize = ftell(file);
	if (filesize < 0) {
		goto fail;
	}
	fseek(file, 0L, SEEK_SET);

	data = mmap(0, filesize, PROT_READ, MAP_PRIVATE, fileno(file), 0);
	if (data == MAP_FAILED) {
		goto fail;
	}

	// decode the data from the file to a lua table
	lua_pushcfunction(L, json_decode);
	lua_pushstring(L, data);
	call_lua_function(L, 1, 1);

	// get the value associated with the key
	lua_pushstring(L, key);
	lua_gettable(L, 1);
	if (lua_isnil(L, -1)) {
		lua_pop(L, 2);
		goto fail;
	}

	value = xstrdup(luaL_checkstring(L, -1));

	// pop the table and the string
	lua_pop(L, 2);

	assert(lua_gettop(L) == 0);

	munmap(data, filesize);

	fclose(file);
	lua_close(L);
	return value;

fail:
	if (data) {
		munmap(data, filesize);
	}
	if (file) {
		fclose(file);
	}
	assert(lua_gettop(L) == 0);
	lua_close(L);
	return NULL;
}

void storage_store(const char *key, const char *value)
{
	long filesize;
	FILE *file = NULL;
	const char *stored_data;
	lua_State *L;

	assert(key);
	assert(value);

	L = luaL_newstate();
	if (!L)
		log_oom_and_exit();

	file = fopen(".storage", "r");
	if (file) {
		fseek(file, 0L, SEEK_END);
		filesize = ftell(file);
		fseek(file, 0L, SEEK_SET);

		if (filesize > 0) {
			// If there is something in the file, we mmap it and decode it to create a table
			int r;
			char *data = mmap(0, filesize, PROT_READ, MAP_PRIVATE, fileno(file), 0);
			if (data == MAP_FAILED) {
				goto finish;
			}

			lua_pushcfunction(L, json_decode);
			lua_pushstring(L, data);
			call_lua_function(L, 1, 1);

			r = munmap(data, filesize);
			if (r < 0) {
				lua_pop(L, 1);
				goto finish;
			}
		}
		fclose(file);
	} else {
		lua_newtable(L);
	}

	file = fopen(".storage", "w");
	if (!file) {
		goto finish;
	}

	// add the new element to the table
	lua_pushstring(L, value);
	lua_setfield(L, -2, key);

	// encode the table in json
	lua_pushcfunction(L, json_encode);
	lua_pushvalue(L, 1);
	call_lua_function(L, 1, 1);

	// store this in the file
	if (lua_isnil(L, -1)) {
		lua_pop(L, 2);
		goto finish;
	}
	stored_data = luaL_checkstring(L, -1);
	fputs(stored_data, file);

	// pop the table and the string
	lua_pop(L, 2);

finish:
	if (file) {
		fclose(file);
	}
	assert(lua_gettop(L) == 0);
	lua_close(L);
}
#endif

