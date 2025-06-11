#include <lua.hpp>
#include <SFML_Time.hpp>

void register_sfTime(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Time>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Time*>();
