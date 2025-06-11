#include <lua.hpp>
#include <SFML_Music.hpp>

void register_MusicEnums(lua_State * l);

void register_sfMusic(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Music>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Music*>();
