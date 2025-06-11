#include <lua.hpp>
#include <SFML_Event.hpp>


void register_sfSizeEvent(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Event::SizeEvent>();

template <>
const char* metaTableName<sf::Event::SizeEvent*>();
