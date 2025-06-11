#include <lua.hpp>
#include <SFML_RenderWindow.hpp>



void register_sfRenderWindow(lua_State * l);

void register_drawableType(const char* name);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::RenderWindow>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::RenderWindow*>();
