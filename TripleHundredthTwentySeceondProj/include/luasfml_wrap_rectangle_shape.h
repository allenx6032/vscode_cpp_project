#include <lua.hpp>
#include <SFML_RectangleShape.hpp>


void register_sfRectangleShape(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::RectangleShape>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::RectangleShape*>();
