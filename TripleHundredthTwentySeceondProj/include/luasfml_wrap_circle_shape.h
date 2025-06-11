#include <lua.hpp>
#include <SFML_CircleShape.hpp>


void register_sfCircleShape(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::CircleShape>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::CircleShape*>();
