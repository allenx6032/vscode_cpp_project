#include <lua.hpp>
#include <SFML_Event.hpp>


void register_sfMouseMoveEvent(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Event::MouseMoveEvent>();

template <>
const char* metaTableName<sf::Event::MouseMoveEvent*>();
