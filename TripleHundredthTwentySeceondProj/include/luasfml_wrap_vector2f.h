#include <lua.hpp>
#include <SFML_Vector2.hpp>

void register_sfVector2f(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Vector2f>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Vector2f*>();

template< >
sf::Vector2f* lua_get(lua_State * l,int i);
