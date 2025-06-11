#include <lua.hpp>
#include <SFML_SoundBuffer.hpp>


void register_sfSoundBuffer(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::SoundBuffer>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::SoundBuffer*>();
