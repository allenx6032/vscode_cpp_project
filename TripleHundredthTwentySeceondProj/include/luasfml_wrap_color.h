#include <lua.hpp>
#include <SFML_Color.hpp>

void register_sfColor(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Color>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Color*>();
