#include <lua.hpp>
#include <SFML_Rect.hpp>

void register_sfIntRect(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::IntRect>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::IntRect*>();
