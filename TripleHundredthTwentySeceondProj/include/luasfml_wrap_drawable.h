#include <lua.hpp>
#include <SFML_Drawable.hpp>

void register_sfDrawable(lua_State * l);


#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Drawable>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<sf::Drawable*>();
