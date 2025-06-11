#include <lua.hpp>
#include <SFML_Texture.hpp>

void register_sfTexture(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Texture>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Texture*>();
