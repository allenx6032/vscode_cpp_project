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
#pragma once

#include <lua.h>

#include "drystal_lua_util.h"
#include "drystal_sound.h"

DECLARE_PUSHPOP(Sound, sound)

int mlua_load_sound(lua_State *L);
int mlua_create_sound(lua_State *L);
int mlua_play_sound(lua_State *L);
int mlua_free_sound(lua_State *L);

