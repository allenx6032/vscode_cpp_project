/**
 * Copyright (c) 2006-2015 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#include "love_wrap_Sound.h"

#include "love_wrap_Filesystem.h"

// Implementations.
#include "love_lullaby_Sound.h"

namespace love
{
namespace sound
{

#define instance() (Module::getInstance<Sound>(Module::M_SOUND))

int w_newSoundData(lua_State *L)
{
	SoundData *t = 0;

	if (lua_isnumber(L, 1))
	{
		int samples = luaL_checkint(L, 1);
		int sampleRate = luaL_optint(L, 2, Decoder::DEFAULT_SAMPLE_RATE);
		int bitDepth = luaL_optint(L, 3, Decoder::DEFAULT_BIT_DEPTH);
		int channels = luaL_optint(L, 4, Decoder::DEFAULT_CHANNELS);

		luax_catchexcept(L, [&](){ t = instance()->newSoundData(samples, sampleRate, bitDepth, channels); });
	}
	// Must be string or decoder.
	else
	{
		// Convert to Decoder, if necessary.
		if (!luax_istype(L, 1, SOUND_DECODER_T))
		{
			w_newDecoder(L);
			lua_replace(L, 1);
		}

		luax_catchexcept(L, [&](){ t = instance()->newSoundData(luax_checkdecoder(L, 1)); });
	}

	luax_pushtype(L, "SoundData", SOUND_SOUND_DATA_T, t);
	t->release();
	return 1;
}

int w_newDecoder(lua_State *L)
{
	love::filesystem::FileData *data = love::filesystem::luax_getfiledata(L, 1);
	int bufferSize = luaL_optint(L, 2, Decoder::DEFAULT_BUFFER_SIZE);

	Decoder *t = nullptr;
	luax_catchexcept(L,
		[&]() { t = instance()->newDecoder(data, bufferSize); },
		[&]() { data->release(); }
	);

	if (t == nullptr)
		return luaL_error(L, "Extension \"%s\" not supported.", data->getExtension().c_str());

	luax_pushtype(L, "Decoder", SOUND_DECODER_T, t);
	t->release();
	return 1;
}

// List of functions to wrap.
static const luaL_Reg functions[] =
{
	{ "newSoundData",  w_newSoundData },
	{ "newDecoder",  w_newDecoder },
	{ 0, 0 }
};

static const lua_CFunction types[] =
{
	luaopen_sounddata,
	luaopen_decoder,
	0
};

extern "C" int luaopen_love_sound(lua_State *L)
{
	Sound *instance = instance();
	if (instance == nullptr)
	{
		luax_catchexcept(L, [&](){ instance = new lullaby::Sound(); });
	}
	else
		instance->retain();


	WrappedModule w;
	w.module = instance;
	w.name = "sound";
	w.flags = MODULE_SOUND_T;
	w.functions = functions;
	w.types = types;

	return luax_register_module(L, w);
}

} // sound
} // love
