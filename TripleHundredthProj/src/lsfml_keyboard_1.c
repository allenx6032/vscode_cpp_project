/*
** EPITECH PROJECT, 2019
** keyboard
** File description:
** keyboard
*/

#include "lua_includes.h"
#include <SFML_Graphics.h>
#include <SFML_Window.h>

int keyboard_key_pressed(lua_State *L)
{
    int key_code = 0;
    int nb = 0;

    if (lua_gettop(L) < 1) {
        luaL_error(L, "Expected (Key)");
        return (0);
    }
    if (lua_isinteger(L, 1)) {
        key_code = lua_tointeger(L, 1);
        nb = sfKeyboard_isKeyPressed(key_code);
        lua_pushboolean(L, nb);
    } else {
        luaL_error(L, "Expected (Int)");
        return (0);
    }
    return (1);
}