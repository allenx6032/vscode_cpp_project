#include "luasfml_wrap_clock.h"
#include <SFML_Clock.hpp>

#include "luasfml_wrap_time.h"

#include "luasfml_marco.hpp"
#include "luasfml_lua_template.hpp"

#define TYPE_TO_BIND sf::Clock
#define NAME_TO_BIND sfClock

IMPLEMENT_LUA_CLASS(sfClock,sf::Clock)

#define FUNCTION_TO_BIND \
X0(restart        , METHOD   ,HAS_RETURN , RET_TYPE(sf::Time)   , restart      )                  \
X0(getElapsedTime , METHOD   ,HAS_RETURN , RET_TYPE(sf::Time)   , getElapsedTime )

#define __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

FUNCTION_TO_BIND

#undef __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

luaL_Reg sfClock_regs[] =
{
    LUA_DEFAULT_CLASS_FUNC(sfClock)
    FUNCTION_TO_BIND
    { NULL, NULL }
};

LUA_REGISTER_CLASS(sfClock)

