#include <lua.hpp>
#include <SFML_Rect.hpp>

void register_sfFloatRect(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::FloatRect>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::FloatRect*>();
