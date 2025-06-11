#include <lua.hpp>
#include <SFML_Sound.hpp>

void register_SoundEnums(lua_State * l);

void register_sfSound(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Sound>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Sound*>();
