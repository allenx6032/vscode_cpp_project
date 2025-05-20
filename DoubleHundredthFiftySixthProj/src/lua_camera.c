/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

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

#include <lua.h>

#include <yoyo_camera.h>
#include <lua_api.h>
#include <yoyo_logging.h>



int ye_lua_create_camera(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create camera: entity is null\n");
        return 0;
    }

    // get x,y,w,h,z
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    int w = luaL_checknumber(L, 4);
    int h = luaL_checknumber(L, 5);
    int z = luaL_checknumber(L, 6);

    ye_add_camera_component(ent, z, (SDL_Rect){x, y, w, h});

    return 0;
}



int ye_lua_camera_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query camera: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->camera->active);
    lua_pushboolean(L, ent->camera->relative);
    lua_pushnumber(L, ent->camera->z);
    lua_pushnumber(L, ent->camera->view_field.x);
    lua_pushnumber(L, ent->camera->view_field.y);
    lua_pushnumber(L, ent->camera->view_field.w);
    lua_pushnumber(L, ent->camera->view_field.h);

    return 7;
}

int ye_lua_camera_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify camera: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->camera->active = lua_toboolean(L, 2);
    }

    if(lua_isboolean(L, 3)){
        ent->camera->relative = lua_toboolean(L, 3);
    }

    if(lua_isnumber(L, 4)){
        ent->camera->z = luaL_checknumber(L, 4);
    }

    if(lua_isnumber(L, 5)){
        ent->camera->view_field.x = luaL_checknumber(L, 5);
    }

    if(lua_isnumber(L, 6)){
        ent->camera->view_field.y = luaL_checknumber(L, 6);
    }

    if(lua_isnumber(L, 7)){
        ent->camera->view_field.w = luaL_checknumber(L, 7);
    }

    if(lua_isnumber(L, 8)){
        ent->camera->view_field.h = luaL_checknumber(L, 8);
    }

    return 0;
}



void ye_lua_camera_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_camera", ye_lua_create_camera);

    // query
    lua_register(L, "ye_lua_camera_query", ye_lua_camera_query);

    // modify
    lua_register(L, "ye_lua_camera_modify", ye_lua_camera_modify);
}