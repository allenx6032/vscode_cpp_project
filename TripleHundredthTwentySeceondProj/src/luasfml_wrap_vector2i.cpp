#include "luasfml_wrap_vector2f.h"
#include "luasfml_wrap_vector2i.h"
#include "luasfml_wrap_vector2u.h"
#include <lua.hpp>
#include <SFML_Vector2.hpp>

#include <lua.hpp>

#include "luasfml_marco.hpp"
#include "luasfml_lua_template.hpp"


template< >
sf::Vector2i* lua_get(lua_State * l,int i)
{
    if(test_type<sf::Vector2f*>(l,i))
    {
	sf::Vector2f* tmp = check_pointertype<sf::Vector2f*>(l,i);
	copy_constructor<sf::Vector2i>(l,sf::Vector2i(tmp->x,tmp->y));
	lua_replace(l,i);
    }
    if(test_type<sf::Vector2u*>(l,i))
    {
	sf::Vector2u* tmp = check_pointertype<sf::Vector2u*>(l,i);
	copy_constructor<sf::Vector2i>(l,sf::Vector2i(tmp->x,tmp->y));
	lua_replace(l,i);
    }
    if(lua_isnumber(l,i) && lua_isnumber(l,i+1))
    {
	copy_constructor<sf::Vector2i>(l,sf::Vector2i(lua_get<int>(l,i),lua_get<int>(l,i+1)));
	lua_replace(l,i);
	lua_remove(l,i+1);
    }
    return check_pointertype<sf::Vector2i*>(l,i);
}


#define TYPE_TO_BIND sf::Vector2i
#define NAME_TO_BIND sfVector2i


#include "luasfml_macro2.hpp"


DEFAULT_CTOR_2(sf::Vector2i,int,NO_POINTER,int,NO_POINTER)

IMPLEMENT_LUA_CLASS(sfVector2i,sf::Vector2i)

#define FUNCTION_TO_BIND \
    X2(__mul          , FUNCTION ,HAS_RETURN , RET_CTOR        , operator*        , TYPE_TO_BIND , IS_POINTER , int        , NO_POINTER) \
    X2(__div          , FUNCTION ,HAS_RETURN , RET_CTOR        , operator/        , TYPE_TO_BIND , IS_POINTER , int        , NO_POINTER) \
    X2(__add          , FUNCTION ,HAS_RETURN , RET_CTOR        , operator+        , TYPE_TO_BIND , IS_POINTER , TYPE_TO_BIND , IS_POINTER) \
    X2(__sub          , FUNCTION ,HAS_RETURN , RET_CTOR        , operator-        , TYPE_TO_BIND , IS_POINTER , TYPE_TO_BIND , IS_POINTER) \
    X2(__eq           , FUNCTION ,HAS_RETURN , RET_TYPE(bool)  , operator==       , TYPE_TO_BIND , IS_POINTER , TYPE_TO_BIND , IS_POINTER) \
    X1(setX           , SETTER   ,NO_RETURN  , RET_NONE        , x                , int        , NO_POINTER )\
    X1(setY           , SETTER   ,NO_RETURN  , RET_NONE        , y                , int        , NO_POINTER )\
    X0(x              , GETTER   ,HAS_RETURN , RET_TYPE(int) , x                )\
    X0(y              , GETTER   ,HAS_RETURN , RET_TYPE(int) , y                )\

#define __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

FUNCTION_TO_BIND

#undef __GO_FOR_IMPLEMENTATION__
#include "luasfml_macro2.hpp"

luaL_Reg sfVector2i_regs[] =
{
    LUA_DEFAULT_CLASS_FUNC(sfVector2i)
    FUNCTION_TO_BIND
    { NULL, NULL }
};

LUA_REGISTER_CLASS(sfVector2i)


