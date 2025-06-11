#include <lua.hpp>
#include <SFML_Event.hpp>


void register_sfMouseButtonEvent(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Event::MouseButtonEvent>();

template <>
const char* metaTableName<sf::Event::MouseButtonEvent*>();
