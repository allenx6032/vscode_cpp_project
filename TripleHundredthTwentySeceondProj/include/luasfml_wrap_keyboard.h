#include <lua.hpp>
#include <SFML_Keyboard.hpp>

void register_KeyBoardEnums(lua_State * l);


void register_sfKeyboard(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Keyboard>();

template <>
const char* metaTableName<sf::Keyboard*>();

template<>
sf::Keyboard::Key lua_get<sf::Keyboard::Key>(lua_State * l,int i);
