#include <lua.hpp>
#include "luasfml_wrap_music.h"
#include "luasfml_wrap_sound.h"
#include "luasfml_wrap_soundbuffer.h"

void registerAudioModule(lua_State * l)
{
    register_MusicEnums(l);
    register_SoundEnums(l);

    register_sfMusic(l);
    register_sfSoundBuffer(l);
    register_sfSound(l);
}

