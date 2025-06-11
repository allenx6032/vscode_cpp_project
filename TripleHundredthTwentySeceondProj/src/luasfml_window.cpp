#include <lua.hpp>
#include "luasfml_wrap_window.h"
#include "luasfml_wrap_videomode.h"
#include "luasfml_wrap_event.h"
#include "luasfml_wrap_mouse.h"
#include "luasfml_wrap_keyboard.h"
#include "luasfml_wrap_mouse_button_event.h"
#include "luasfml_wrap_mouse_move_event.h"
#include "luasfml_wrap_mouse_wheel_event.h"
#include "luasfml_wrap_size_event.h"
#include "luasfml_wrap_key_event.h"


void registerWindowModule(lua_State * l)
{
    register_EventEnums(l);
    register_KeyBoardEnums(l);
    register_MouseEnums(l);
    register_WindowEnums(l);

    register_sfWindow(l);
    register_sfVideoMode(l);
    register_sfKeyboard(l);
    register_sfMouse(l);
    register_sfEvent(l);
    register_sfMouseButtonEvent(l);
    register_sfMouseMoveEvent(l);
    register_sfMouseWheelEvent(l);
    register_sfSizeEvent(l);
    register_sfKeyEvent(l);

}

