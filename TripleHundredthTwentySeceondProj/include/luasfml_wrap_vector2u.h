#include <lua.hpp>
#include <SFML_Vector2.hpp>

void register_sfVector2u(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Vector2u>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Vector2u*>();

template< >
sf::Vector2u* lua_get(lua_State * l,int i);
