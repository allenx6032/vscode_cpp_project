#include <lua.hpp>
#include <SFML_Font.hpp>

void register_sfFont(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Font>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Font*>();
