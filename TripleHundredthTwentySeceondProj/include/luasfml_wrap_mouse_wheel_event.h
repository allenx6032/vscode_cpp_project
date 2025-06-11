#include <lua.hpp>
#include <SFML_Event.hpp>


void register_sfMouseWheelEvent(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Event::MouseWheelEvent>();

template <>
const char* metaTableName<sf::Event::MouseWheelEvent*>();
