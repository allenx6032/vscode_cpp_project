#include <lua.hpp>
#include <SFML_Sprite.hpp>

void register_sfSprite(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Sprite>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Sprite*>();
