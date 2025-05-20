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

#include <yoyo_button.h>
#include <lua_api.h>
#include <yoyo_logging.h>



int ye_lua_create_button(lua_State *L) {
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not create button component: parent entity is null\n");
        return 0;
    }

    // get x,y,w,h
    int x = luaL_checknumber(L, 2);
    int y = luaL_checknumber(L, 3);
    int w = luaL_checknumber(L, 4);
    int h = luaL_checknumber(L, 5);

    ye_add_button_component(ent, (struct ye_rectf){x, y, w, h});

    return 0;
}



int ye_lua_button_query(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not query button component: parent entity is null\n");
        return 0;
    }

    lua_pushboolean(L, ent->button->active);
    lua_pushboolean(L, ent->button->relative);
    lua_pushnumber(L, ent->button->rect.x);
    lua_pushnumber(L, ent->button->rect.y);
    lua_pushnumber(L, ent->button->rect.w);
    lua_pushnumber(L, ent->button->rect.h);

    // we are going to wrap these as direct calls from lua, because
    // the state gets reset for clicked after observing the event,
    // and that click can be observed and reset by calls to _query 
    // that are not in the same frame as the event
    //
    // lua_pushboolean(L, ye_button_hovered(ent));
    // lua_pushboolean(L, ye_button_pressed(ent));
    // lua_pushboolean(L, ye_button_clicked(ent));

    return 9;
}

int ye_lua_button_modify(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);
    
    if(ent == NULL) {
        ye_logf(error, "could not modify button: entity is null\n");
        return 0;
    }

    if(lua_isboolean(L, 2)){
        ent->button->active = lua_toboolean(L, 2);
    }

    if(lua_isboolean(L, 3)){
        ent->button->relative = lua_toboolean(L, 3);
    }

    if(lua_isnumber(L, 4)){
        ent->button->rect.x = luaL_checknumber(L, 5);
    }

    if(lua_isnumber(L, 5)){
        ent->button->rect.y = luaL_checknumber(L, 6);
    }

    if(lua_isnumber(L, 6)){
        ent->button->rect.w = luaL_checknumber(L, 7);
    }

    if(lua_isnumber(L, 7)){
        ent->button->rect.h = luaL_checknumber(L, 8);
    }

    // we are NOT allowed to modify whether or not a button is hovered, pressed, or clicked
    // TODO: any good reason to add this in the future?

    return 0;
}



int ye_lua_button_check_state(lua_State *L){
    struct ye_entity * ent = lua_touserdata(L, 1);

    if(ent == NULL) {
        ye_logf(error, "could not check button state: entity is null\n");
        return 0;
    }

    int query_type = luaL_checknumber(L, 2); // the type of query we are going to perform

    // printf("query type: %d\n", query_type);

    switch(query_type){
        case 1: // hovered
            lua_pushboolean(L, ye_button_hovered(ent));
            break;
        case 2: // pressed
            lua_pushboolean(L, ye_button_pressed(ent));
            break;
        case 3: // clicked
            lua_pushboolean(L, ye_button_clicked(ent));
            break;
        default:
            ye_logf(error, "could not check button state: invalid query type\n");
            return 0;
    }

    return 1;
}



void ye_lua_button_register(lua_State *L) {
    // init
    lua_register(L, "ye_lua_create_button", ye_lua_create_button);

    // query
    lua_register(L, "ye_lua_button_query", ye_lua_button_query);

    // modify
    lua_register(L, "ye_lua_button_modify", ye_lua_button_modify);

    // check state
    lua_register(L, "ye_lua_button_check_state", ye_lua_button_check_state);
}