#include <lua.hpp>
#include "luasfml_window.h"
#include "luasfml_system.h"
#include "luasfml_graphics.h"
#include "luasfml_audio.h"

void registerSFMLModule(lua_State * l)
{
    registerSystemModule(l);
    registerWindowModule(l);
    registerGraphicsModule(l);
    registerAudioModule(l);
}

