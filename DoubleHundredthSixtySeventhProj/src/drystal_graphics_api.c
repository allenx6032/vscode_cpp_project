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
#include <lua.h>
#include <lauxlib.h>

#include "drystal_module.h"
#include "drystal_display_bind.h"
#include "drystal_display.h"
#include "drystal_buffer.h"
#include "drystal_camera_bind.h"
#include "drystal_shader_bind.h"
#include "drystal_buffer_bind.h"
#include "drystal_graphics_api.h"
#include "drystal_util.h"

BEGIN_MODULE(graphics)
	DECLARE_FUNCTION(show_cursor)

	DECLARE_FUNCTION(resize)
	DECLARE_FUNCTION(set_title)
	DECLARE_FUNCTION(set_fullscreen)
	DECLARE_FUNCTION(screen2scene)

	/* DISPLAY SURFACE */
	DECLARE_FUNCTION(load_surface)
	DECLARE_FUNCTION(new_surface)

	/* DISPLAY DRAWERS */
	DECLARE_FUNCTION(draw_background)
	DECLARE_FUNCTION(draw_point)
	DECLARE_FUNCTION(draw_point_tex)
	DECLARE_FUNCTION(draw_line)
	DECLARE_FUNCTION(draw_triangle)
	DECLARE_FUNCTION(draw_surface)
	DECLARE_FUNCTION(draw_quad)

	/* DISPLAY SETTERS */
	DECLARE_FUNCTION(set_color)
	DECLARE_FUNCTION(set_alpha)
	DECLARE_FUNCTION(set_blend_mode)

	BEGIN_CLASS(surface)
		ADD_METHOD(surface, set_filter)
		ADD_METHOD(surface, draw_on)
		ADD_METHOD(surface, draw_from)
		ADD_METHOD(surface, get_pixel)
		ADD_GC(free_surface)
	REGISTER_CLASS_WITH_INDEX(surface, "Surface")

	DECLARE_FUNCTION(new_buffer)
	DECLARE_FUNCTION(use_default_buffer)
	BEGIN_CLASS(buffer)
	    ADD_METHOD(buffer, use)
	    ADD_METHOD(buffer, draw)
	    ADD_METHOD(buffer, reset)
	    ADD_METHOD(buffer, upload_and_free)
	    ADD_GC(free_buffer)
	REGISTER_CLASS(buffer, "Buffer")

	DECLARE_FUNCTION(new_shader)
	DECLARE_FUNCTION(use_default_shader)
	BEGIN_CLASS(shader)
	    ADD_METHOD(shader, use)
	    ADD_METHOD(shader, feed)
	    ADD_GC(free_shader)
	REGISTER_CLASS(shader, "Shader")

	{
		// make sure we don't free the screen until the next resize
		push_surface(L, display_get_screen());
		lua_setfield(L, LUA_REGISTRYINDEX, "screen");
	}

	BEGIN_ENUM()
		ADD_CONSTANT("default", BLEND_DEFAULT)
		ADD_CONSTANT("alpha", BLEND_ALPHA)
		ADD_CONSTANT("add", BLEND_ADD)
		ADD_CONSTANT("mult", BLEND_MULT)
		ADD_CONSTANT("trilinear", BLEND_DEFAULT)
	REGISTER_ENUM("blends")

	BEGIN_ENUM()
		ADD_CONSTANT("default", FILTER_DEFAULT)
		ADD_CONSTANT("nearest", FILTER_NEAREST)
		ADD_CONSTANT("linear", FILTER_LINEAR)
		ADD_CONSTANT("bilinear", FILTER_BILINEAR)
		ADD_CONSTANT("trilinear", FILTER_TRILINEAR)
	REGISTER_ENUM("filters")

	{
		// camera
		lua_newtable(L);
		luaL_newmetatable(L, "__camera_class");
		lua_pushcfunction(L, mlua_camera__newindex);
		lua_setfield(L, -2, "__newindex");
		lua_pushcfunction(L, mlua_camera__index);
		lua_setfield(L, -2, "__index");
		lua_setmetatable(L, -2);
		PUSH_FUNC("reset", camera_reset);
		PUSH_FUNC("push", camera_push);
		PUSH_FUNC("pop", camera_pop);

		// glue it on drystal table
		lua_setfield(L, -2, "camera");
	}
END_MODULE()

int graphics_index(lua_State* L)
{
	const char * name = luaL_checkstring(L, 2);
	if (streq(name, "screen")) {
		Surface* surf = display_get_screen();
		if (surf) {
			push_surface(L, surf);
			return 1;
		}
	} else if (streq(name, "current_draw_on")) {
		Surface* surf = display_get_draw_on();
		if (surf) {
			push_surface(L, surf);
			return 1;
		}
	} else if (streq(name, "current_draw_from")) {
		Surface* surf = display_get_draw_from();
		if (surf) {
			push_surface(L, surf);
			return 1;
		}
	}

	return 0;
}

