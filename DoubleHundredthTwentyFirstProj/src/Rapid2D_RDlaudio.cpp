// Copyright 2016 KeNan Liu
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Rapid2D_RDLua.h"
#include "Rapid2D_RDLog.h"
#include "Rapid2D_RDAudio.h"

#define RD_AUDIO_BUFFER "Rapid2D_CAudioBuffer"
#define RD_AUDIO_SOURCE "Rapid2D_CAudioPlayer"

static int lnewBuffer(lua_State * L)
{
#if defined(RD_DEBUG)
    // check args
    int argc = lua_gettop(L);
    if (argc != 1) {
        return luaL_error(L, "[Lua Error]: %s newBuffer() fail, has wrong number of arguments",
                          RD_LIB_AUDIO);
    }
#endif
    
    // FilePath
	const char *path = lua_tostring(L, 1);
    ALuint bufferID = RDAudio_LoadFile(path);
    
    // create userdata
	ALuint *userdata = (ALuint *)lua_newuserdata(L, sizeof(ALuint));
    *userdata = bufferID;
	// set metatable
    luaL_getmetatable(L, RD_AUDIO_BUFFER);
    lua_setmetatable(L, -2);
    return 1;// number of return values
}

static int lnewSource(lua_State * L)
{
    ALuint sourceID;
    // grab a source ID from openAL
    alGenSources(1, &sourceID);
    if (alGetError() != AL_NO_ERROR) {
        return luaL_error(L, "[Lua Error]: %s newSource() fail",
                          RD_LIB_AUDIO);
    }
    
    // create userdata
    ALuint *userdata = (ALuint *)lua_newuserdata(L, sizeof(ALuint));
    *userdata = sourceID;
    // set metatable
    luaL_getmetatable(L, RD_AUDIO_SOURCE);
    lua_setmetatable(L, -2);
    return 1;// number of return values
}

/******************** for buffer metatable ********************/
static int lBufferGC(lua_State *L)
{
    ALuint *bufferID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_BUFFER);
    alDeleteBuffers(1, bufferID);
    return 0;// number of return values
}

static const struct luaL_Reg meta_buffer [] = {
    {"__gc", lBufferGC},
    {NULL, NULL}
};

/******************** for source metatable ********************/
static int lSourceGC(lua_State *L)
{
    ALuint *sourceID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_SOURCE);
    alDeleteSources(1, sourceID);
    return 0;// number of return values
}

static int lSourcePlay2d(lua_State *L)
{
    ALuint *sourceID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_SOURCE);
    ALuint *bufferID = (ALuint *)luaL_checkudata(L, 2, RD_AUDIO_BUFFER);
    int isLoop = lua_toboolean(L, 3);
    
    // attach the buffer to the source
    alSourcei(*sourceID, AL_BUFFER, *bufferID);
    // set loop sound
    alSourcei(*sourceID, AL_LOOPING, isLoop);
    alSourcePlay(*sourceID);
    // clean error code
    alGetError();
    return 0;
}

static int lSourcePause(lua_State *L)
{
    ALuint *sourceID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_SOURCE);
    alSourcePause(*sourceID);
    return 0;
}

static int lSourceResume(lua_State *L)
{
    ALuint *sourceID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_SOURCE);
    alSourcePlay(*sourceID);
    return 0;
}

static int lSourceStop(lua_State *L)
{
    ALuint *sourceID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_SOURCE);
    alSourceStop(*sourceID);
    return 0;
}

static int lSourceSetVolume(lua_State *L)
{
    ALuint *sourceID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_SOURCE);
    lua_Number volume = lua_tonumber(L, 2);
    if (volume < 0.0f) {
        volume = 0.0f;
    }
    if (volume > 1.0f) {
        volume = 1.0f;
    }
    alSourcef(*sourceID, AL_GAIN, volume);
    return 0;
}

static int lSourceGetStat(lua_State *L)
{
    ALuint *sourceID = (ALuint *)luaL_checkudata(L, 1, RD_AUDIO_SOURCE);
    ALint stat;
    alGetSourcei(*sourceID, AL_SOURCE_STATE, &stat);
    switch (stat) {
        case AL_INITIAL:
            lua_pushinteger(L, 1);
            break;
        case AL_PLAYING:
            lua_pushinteger(L, 2);
            break;
        case AL_PAUSED:
            lua_pushinteger(L, 3);
            break;
        case AL_STOPPED:
            lua_pushinteger(L, 4);
            break;
        default:
            return luaL_error(L, "[Lua Error]: %s getStat() fail",
                              RD_AUDIO_SOURCE);
    }
    return 1;
}

static const struct luaL_Reg meta_source [] = {
    {"__gc", lSourceGC},
    {"play2d", lSourcePlay2d},
    {"pause", lSourcePause},
    {"resume", lSourceResume},
    {"stop", lSourceStop},
    {"setVolume", lSourceSetVolume},
    {"getStat", lSourceGetStat},
    {NULL, NULL}
};

static const struct luaL_Reg audio_funcs [] = {
    {"newBuffer", lnewBuffer},
    {"newSource", lnewSource},
    {NULL, NULL}
};

// extern func for register library
LUAMOD_API int luaopen_audio(lua_State *L)
{
    luaL_newmetatable(L, RD_AUDIO_BUFFER);
    /* metatable.__index = metatable */
    lua_pushvalue(L, -1);  /* duplicates the metatable */
    lua_setfield(L, -2, "__index");
    /* add method to metatable */
    luaL_setfuncs(L, meta_buffer, 0);
    lua_pop(L, 1);  /* pop new metatable */
    
    luaL_newmetatable(L, RD_AUDIO_SOURCE);
    /* metatable.__index = metatable */
    lua_pushvalue(L, -1);  /* duplicates the metatable */
    lua_setfield(L, -2, "__index");
    /* add method to metatable */
    luaL_setfuncs(L, meta_source, 0);
    lua_pop(L, 1);  /* pop new metatable */
    
    // binding userdata to new metatable
    luaL_newlib(L, audio_funcs);
    return 1;
}
