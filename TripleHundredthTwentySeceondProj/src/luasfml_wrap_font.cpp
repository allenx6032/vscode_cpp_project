#include "luasfml_wrap_font.h"

#include <lua.hpp>

#include "luasfml_marco.hpp"
#include "luasfml_lua_template.hpp"


#define TYPE_TO_BIND sf::Font
#define NAME_TO_BIND sfFont

IMPLEMENT_LUA_CLASS(sfFont,sf::Font)

#define FUNCTION_TO_BIND \
X1(loadFromFile   , METHOD   ,HAS_RETURN  , RET_TYPE(bool)         , loadFromFile , std::string   , NO_POINTER )\

#define __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

FUNCTION_TO_BIND

#undef __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

luaL_Reg sfFont_regs[] =
{
    LUA_DEFAULT_CLASS_FUNC(sfFont)
    FUNCTION_TO_BIND
    { NULL, NULL }
};

LUA_REGISTER_CLASS(sfFont)
