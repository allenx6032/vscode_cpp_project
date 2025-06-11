#include <lua.hpp>
#include <SFML_Text.hpp>

void register_TextEnums(lua_State * l);

void register_sfText(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Text>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Text*>();
