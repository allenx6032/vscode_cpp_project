#include "luasfml_tilemap.h"
#include <lua.hpp>



void register_sfeTileMap(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<TileMap>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<TileMap*>();
