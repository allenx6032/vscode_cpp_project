#include <lua.hpp>
#include <SFML_Vector3.hpp>

void register_sfVector3i(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Vector3i>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Vector3i*>();
