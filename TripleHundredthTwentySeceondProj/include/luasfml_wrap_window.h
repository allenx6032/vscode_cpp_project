#include <lua.hpp>
#include <SFML_Window_Window.hpp>


void register_WindowEnums(lua_State * l);


void register_sfWindow(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Window>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Window*>();
