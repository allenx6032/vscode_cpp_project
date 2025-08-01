/*
** EPITECH PROJECT, 2019
** main
** File description:
** main
*/

#include "lsfml.h"
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "engine.h"
#include <unistd.h>
#include <SFML_Graphics.h>
#include <SFML_Window.h>
#include <SFML_Audio.h>
#include <SFML_System.h>

lua_State *init_lua(void)
{
    lua_State *lua = luaL_newstate();

    luaL_openlibs(lua);
    luaopen_lsfml(lua);
    return (lua);
}

void delete_lua(lua_State *L)
{
    lua_gc(L, LUA_GCCOLLECT, 0);
    lua_close(L);
}

void run_file(lua_State *lua, char const *filename)
{
    int len = -1;
    char const *error = 0;

    if (luaL_dofile(lua, filename) != LUA_OK) {
        error = lua_tostring(lua, -1);
        while (error[++len]);
        write(1, "[ERROR] ", 9);
        write(1,  error, len);
        write(1, "\n", 1);
    }
}

int pcall(lua_State *lua, int args, int result)
{
    return lua_pcall(lua, args, result, 0);
}

int main(int ac UNUSED, char **av UNUSED, char **env)
{
    int code = 0;

    if (env == NULL || env[0] == NULL) {
        write(1, "Environement needs to be set !\n", 31);
        return (84);
    }
    lua_State *lua = init_lua();
    sfRenderWindow *window = sfRenderWindow_create(
    (sfVideoMode){1920, 1080, 32}, "Enter The Lab", sfClose, 0);
    sfRenderWindow **win = (sfRenderWindow **)
    lua_newuserdata(lua, sizeof(sfRenderWindow *));

    sfRenderWindow_setKeyRepeatEnabled(window, sfFalse);
    *win = window;
    lua_setglobal(lua, "window");
    run_file(lua, "lua_script/startup.lua");
    code = engine(lua, window);
    delete_lua(lua);
    return (code);
}