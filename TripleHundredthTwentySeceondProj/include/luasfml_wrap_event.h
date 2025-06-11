#include <lua.hpp>
#include <SFML_Event.hpp>

void register_EventEnums(lua_State * l);


void register_sfEvent(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Event>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Event*>();
