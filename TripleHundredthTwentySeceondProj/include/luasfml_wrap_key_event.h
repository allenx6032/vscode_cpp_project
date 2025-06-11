#include <lua.hpp>
#include <SFML_Event.hpp>


void register_sfKeyEvent(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Event::KeyEvent>();

template <>
const char* metaTableName<sf::Event::KeyEvent*>();
