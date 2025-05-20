/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
    Copyright (C) 2023  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LUA_API_H
#define LUA_API_H

#include <lua.h>
// #include <lua/lualib.h>
// #include <lua/lauxlib.h>
#include <lua_script.h>

/**
 * @brief Takes in a lua state and registers the engine API with it.
 * 
 * @param state 
 */
void ye_register_lua_scripting_api(lua_State *state);

/*
    Callbacks (lua_api_callbacks.c)
*/
void ye_run_lua_on_mount(struct ye_component_lua_script *script);
void ye_run_lua_on_unmount(struct ye_component_lua_script *script);
void ye_run_lua_on_update(struct ye_component_lua_script *script);


//////////////////////////////////////////////////////////////////////////////

/*
    ECS LUA INTERFACES
*/

// Entity
void ye_lua_entity_register(lua_State *L);

// Transform
void ye_lua_transform_register(lua_State *L);

// Camera
void ye_lua_camera_register(lua_State *L);

// Renderer
void ye_lua_renderer_register(lua_State *L);

// Button
void ye_lua_button_register(lua_State *L);

//
void ye_lua_script_ext_register(lua_State *L);

void ye_lua_tag_register(lua_State *L);

void ye_lua_collider_register(lua_State *L);

void ye_lua_physics_register(lua_State *L);


int ye_lua_script_ext_query(lua_State *L);

int ye_lua_script_ext_modify(lua_State *L);

int ye_timer_lua_resolve(struct ye_timer * timer);

//////////////////////////////////////////////////////////////////////////////

/*
    Events dispatched from engine
*/

void ye_run_lua_on_collision(struct ye_component_lua_script *script, struct ye_entity *entity1, struct ye_entity *entity2);
void ye_run_lua_on_trigger_enter(struct ye_component_lua_script *script, struct ye_entity *entity1, struct ye_entity *entity2);

//////////////////////////////////////////////////////////////////////////////

/*
    Cross state manipulation
*/

int ye_invoke_cross_state_function(lua_State* L);
int ye_write_cross_state_value(lua_State* L);
int ye_read_cross_state_value(lua_State* L);

//////////////////////////////////////////////////////////////////////////////

/*
    Misc lua operations
*/

int ye_lua_remove_component(lua_State* L);

//////////////////////////////////////////////////////////////////////////////

/*
    Subsystems
*/

int ye_lua_audio_register(lua_State *L);
int ye_lua_scene_register(lua_State *L);
int ye_lua_timer_register(lua_State *L);
int ye_lua_input_register(lua_State *L);

//////////////////////////////////////////////////////////////////////////////

#endif