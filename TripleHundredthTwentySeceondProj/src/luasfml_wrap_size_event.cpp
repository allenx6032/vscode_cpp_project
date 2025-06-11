#include "luasfml_wrap_size_event.h"
#include <SFML_Event.hpp>

#include <lua.hpp>

#include "luasfml_marco.hpp"
#include "luasfml_lua_template.hpp"


#define TYPE_TO_BIND sf::Event::SizeEvent
#define NAME_TO_BIND sfSizeEvent

IMPLEMENT_LUA_CLASS(sfSizeEvent,sf::Event::SizeEvent)

#define FUNCTION_TO_BIND \
    X0(width      , GETTER   ,HAS_RETURN , RET_TYPE(unsigned int) , width    )\
    X0(height     , GETTER   ,HAS_RETURN , RET_TYPE(unsigned int) , height   )\

#define __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

FUNCTION_TO_BIND

#undef __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

luaL_Reg sfSizeEvent_regs[] =
{
    LUA_DEFAULT_CLASS_FUNC(sfSizeEvent)
    FUNCTION_TO_BIND
    { NULL, NULL }
};

LUA_REGISTER_CLASS(sfSizeEvent)

