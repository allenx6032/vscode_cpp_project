#include <lua.hpp>
#include <SFML_Mouse.hpp>

void register_MouseEnums(lua_State * l);


void register_sfMouse(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Mouse>();

template <>
const char* metaTableName<sf::Mouse*>();

template<>
sf::Mouse::Button lua_get<sf::Mouse::Button>(lua_State * l,int i);
