#include "luasfml_wrap_soundbuffer.h"

#include "luasfml_wrap_time.h"

#include "luasfml_marco.hpp"
#include "luasfml_lua_template.hpp"

#define TYPE_TO_BIND sf::SoundBuffer
#define NAME_TO_BIND sfSoundBuffer

IMPLEMENT_LUA_CLASS(sfSoundBuffer,sf::SoundBuffer)

#define FUNCTION_TO_BIND \
    X1(loadFromFile     , METHOD   ,HAS_RETURN  , RET_TYPE(bool)      , loadFromFile , std::string   , NO_POINTER )\
    X0(getDuration      , METHOD   ,HAS_RETURN , RET_TYPE(sf::Time)   , getDuration )\


#define __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

FUNCTION_TO_BIND

#undef __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

luaL_Reg sfSoundBuffer_regs[] =
{
    LUA_DEFAULT_CLASS_FUNC(sfSoundBuffer)
    FUNCTION_TO_BIND
    { NULL, NULL }
};

LUA_REGISTER_CLASS(sfSoundBuffer)
