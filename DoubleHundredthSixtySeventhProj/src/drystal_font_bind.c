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
#include <lauxlib.h>

#include "drystal_font.h"
#include "drystal_font_bind.h"
#include "drystal_lua_util.h"

IMPLEMENT_PUSHPOP(Font, font)

int mlua_draw_font(lua_State* L)
{
	assert(L);

	Font* font = pop_font(L, 1);
	const char* text = luaL_checkstring(L, 2);
	lua_Number x = luaL_checknumber(L, 3);
	lua_Number y = luaL_checknumber(L, 4);
	Alignment alignment = (Alignment) luaL_optinteger(L, 5, ALIGN_LEFT);
	font_draw(font, text, x, y, alignment);
	return 0;
}

int mlua_draw_plain_font(lua_State* L)
{
	assert(L);

	Font* font = pop_font(L, 1);
	const char* text = luaL_checkstring(L, 2);
	lua_Number x = luaL_checknumber(L, 3);
	lua_Number y = luaL_checknumber(L, 4);
	font_draw_plain(font, text, x, y);
	return 0;
}

int mlua_load_font(lua_State* L)
{
	assert(L);

	const char* filename = luaL_checkstring(L, 1);
	lua_Number size = luaL_checknumber(L, 2);
	Font* font = font_load(filename, size, 32, 96);
	if (font) {
		push_font(L, font);
		return 1;
	}
	return luaL_fileresult(L, 0, filename);
}

int mlua_sizeof_font(lua_State* L)
{
	assert(L);

	Font* font = pop_font(L, 1);
	const char* text = luaL_checkstring(L, 2);
	lua_Number w, h;
	font_get_textsize(font, text, &w, &h, -1);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

int mlua_sizeof_plain_font(lua_State* L)
{
	assert(L);

	Font* font = pop_font(L, 1);
	const char* text = luaL_checkstring(L, 2);
	lua_Number w, h;
	font_get_textsize_plain(font, text, &w, &h);
	lua_pushnumber(L, w);
	lua_pushnumber(L, h);
	return 2;
}

int mlua_free_font(lua_State* L)
{
	assert(L);

	Font* font = pop_font(L, 1);
	font_free(font);
	return 0;
}

