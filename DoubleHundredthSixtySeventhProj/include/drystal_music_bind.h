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

#include "drystal_music.h"
#include "drystal_lua_util.h"

DECLARE_PUSHPOP(Music, music)

int mlua_load_music(lua_State *L);
int mlua_play_music(lua_State *L);
int mlua_stop_music(lua_State *L);
int mlua_pause_music(lua_State *L);
int mlua_free_music(lua_State *L);
int mlua_set_pitch_music(lua_State *L);
int mlua_set_volume_music(lua_State *L);

