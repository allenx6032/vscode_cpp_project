/*
    This file is a part of yoyoengine. (https://github.com/zoogies/yoyoengine)
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

#include <lua_script.h>
#include <lua_api.h>
#include <yoyo_logging.h>



int ye_lua_create_lua_script(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create lua_script component: entity is null\n");
        return 0;
    }

    char * script = luaL_checkstring(L, 2);

    ye_add_lua_script_component(ent, script);

    return 0;
}



int ye_lua_script_ext_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query lua_script: entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->lua_script->active);
    lua_pushstring(L, ent->lua_script->script_handle);

    return 2;
}

int ye_lua_script_ext_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify lua_script: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->lua_script->active = lua_toboolean(L, 2);
    }

    if(lua_isstring(L, 3)){
        /*
            TODO: idk if this is a good way to handle this,
            or if we should even expose this variable at all,
            but lets just remove the script and recreate it with
            the new path for now.
        */
        ye_remove_lua_script_component(ent);
        ye_add_lua_script_component(ent, lua_tostring(L, 3));
    }

    return 0;
}



void ye_lua_script_ext_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_lua_script", ye_lua_create_lua_script);

    // query
    lua_register(L, "ye_lua_lua_script_query", ye_lua_script_ext_query);

    // modify
    lua_register(L, "ye_lua_lua_script_modify", ye_lua_script_ext_modify);
}