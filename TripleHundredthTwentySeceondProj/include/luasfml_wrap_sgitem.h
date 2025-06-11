#include "luasfml_SGItem.h"
#include <lua.hpp>

void register_sfeSGItem(lua_State * l);

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<SGItem>();

#include "luasfml_lua_template.hpp"
template <>
const char* metaTableName<SGItem*>();
