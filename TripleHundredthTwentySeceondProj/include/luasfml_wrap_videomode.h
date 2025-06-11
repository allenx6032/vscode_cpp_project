#include <lua.hpp>
#include <SFML_VideoMode.hpp>



void register_sfVideoMode(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::VideoMode>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::VideoMode*>();
