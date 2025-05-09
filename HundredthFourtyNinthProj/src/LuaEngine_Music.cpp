#include "LuaEngine_include.h"

/*
module Music
	load(filename: string): MusicMedia
	loadmem(data: string): MusicMedia
	loadChunk(filename: string): ChunkMedia
	loadChunkmem(data: string): ChunkMedia
	play(music: MusicMedia, [loops: int])  Loops default to 1 time
	playChunk(chunk: ChunkMedia, channel: int, [loops: int])  Loops default to 1 time.
	fadeIn(music: MusicMedia, ms: int, [loops: int]) Loops default to 1.
	fadeInPos(music: MusicMedia, ms: int, pos: number, [loops: int]) Loops default to 1.
	fadeOut(ms: int)
	pause()
	resume()
	rewind()
	setPos(pos: number)
	stop()
*/

int media_close(lua_State* L)
{
	auto music = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineMusicMedia");
	Mix_FreeMusic(music);
	return 0;
}

void put_media(lua_State* L, Mix_Music* music)
{
	lua_newpointer(L, music);
	if (luaL_newmetatable(L, "LuaEngineMusicMedia"))
	{
		lua_setfield_function(L, "__gc", media_close);
		lua_newtable(L);

		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int chunk_close(lua_State* L)
{
	auto chunk = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineChunkMedia");
	Mix_FreeMusic(chunk);
	return 0;
}

void put_chunk(lua_State* L, Mix_Chunk* chunk)
{
	lua_newpointer(L, chunk);
	if (luaL_newmetatable(L, "LuaEngineChunkMedia"))
	{
		lua_setfield_function(L, "__gc", chunk_close);
		lua_newtable(L);

		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int music_load(lua_State* L)
{
	const char* filename = luaL_checkstring(L, 1);
	Mix_Music* music = Mix_LoadMUS(filename);
	if (!music)
	{
		return MixError(L, Mix_LoadMUS);
	}
	put_media(L, music);
	return 1;
}

int music_loadmem(lua_State* L)
{
	size_t len;
	const char* data = luaL_checklstring(L, 1, &len);
	SDL_RWops* src = SDL_RWFromConstMem(data, len);
	Mix_Music* music = Mix_LoadMUS_RW(src, 0);
	SDL_RWclose(src);
	if (!music)
	{
		return MixError(L, Mix_LoadMUS_RW);
	}
	put_media(L, music);
	return 1;
}

int music_loadchunk(lua_State* L)
{
	const char* filename = luaL_checkstring(L, 1);
	Mix_Chunk* chunk = Mix_LoadWAV(filename);
	if (!chunk)
	{
		return MixError(L, Mix_LoadWAV);
	}
	put_chunk(L, chunk);
	return 1;
}

int music_loadchunkmem(lua_State* L)
{
	size_t len;
	const char* data = luaL_checklstring(L, 1, &len);
	SDL_RWops* src = SDL_RWFromConstMem(data, len);
	Mix_Chunk* chunk = Mix_LoadWAV_RW(src, 0);
	SDL_RWclose(src);
	if (!chunk)
	{
		return MixError(L, Mix_LoadWAV_RW);
	}
	put_chunk(L, chunk);
	return 1;
}

int music_play(lua_State* L)
{
	auto music = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineMusicMedia");
	int loops = 0;
	if (!lua_isnone(L, 2))
	{
		loops = luaL_checkinteger(L, 2);
	}
	if (Mix_PlayMusic(music, loops) != 0)
	{
		return MixError(L, Mix_PlayMusic);
	}
	return 0;
}

int music_fadein(lua_State* L)
{
	auto music = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineMusicMedia");
	int ms = luaL_checkinteger(L, 2);
	int loops = 0;
	if (!lua_isnone(L, 3))
	{
		loops = luaL_checkinteger(L, 3);
	}
	if (Mix_FadeInMusic(music, loops, ms) != 0)
	{
		return MixError(L, Mix_FadeInMusic);
	}
	return 0;
}

int music_fadeinpos(lua_State* L)
{
	auto music = lua_checkpointer<Mix_Music>(L, 1, "LuaEngineMusicMedia");
	int ms = luaL_checkinteger(L, 2);
	double pos = luaL_checknumber(L, 3);
	int loops = 0;
	if (!lua_isnone(L, 4))
	{
		loops = luaL_checkinteger(L, 4);
	}
	if (Mix_FadeInMusicPos(music, loops, ms, pos) != 0)
	{
		return MixError(L, Mix_FadeInMusicPos);
	}
	return 0;
}

int music_fadeout(lua_State* L)
{
	int ms = luaL_checkinteger(L, 1);
	if (Mix_FadeOutMusic(ms) != 0)
	{
		return MixError(L, Mix_FadeOutMusic);
	}
	return 0;
}

int music_playchunk(lua_State* L)
{
	auto chunk = lua_checkpointer<Mix_Chunk>(L, 1, "LuaEngineChunkMedia");
	int channel = luaL_checkinteger(L, 2);
	int loops = 0;
	if (!lua_isnone(L, 3))
	{
		loops = luaL_checkinteger(L, 3);
	}
	int ret = Mix_PlayChannel(channel, chunk, loops);
	if (ret == -1)
	{
		return MixError(L, Mix_PlayChannel);
	}
	lua_pushinteger(L, ret);
	return 1;
}

int music_pause(lua_State* L)
{
	Mix_PauseMusic();
	return 0;
}

int music_resume(lua_State* L)
{
	Mix_ResumeMusic();
	return 0;
}

int music_stop(lua_State* L)
{
	Mix_HaltMusic();
	return 0;
}

int music_rewind(lua_State* L)
{
	Mix_RewindMusic();
	return 0;
}

int music_setpos(lua_State* L)
{
	double pos = luaL_checknumber(L, 1);
	if (Mix_SetMusicPosition(pos) != 0)
	{
		return MixError(L, Mix_SetMusicPosition);
	}
	return 0;
}

void InitMusic(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_newtable(L);
	lua_setfield_function(L, "load", music_load);
	lua_setfield_function(L, "loadmem", music_loadmem);
	lua_setfield_function(L, "loadChunk", music_loadchunk);
	lua_setfield_function(L, "loadChunkmem", music_loadchunkmem);
	lua_setfield_function(L, "play", music_play);
	lua_setfield_function(L, "fadeIn", music_fadein);
	lua_setfield_function(L, "fadeInPos", music_fadeinpos);
	lua_setfield_function(L, "playChunk", music_playchunk);
	lua_setfield_function(L, "fadeOut", music_fadeout);
	lua_setfield_function(L, "pause", music_pause);
	lua_setfield_function(L, "resume", music_resume);
	lua_setfield_function(L, "stop", music_stop);
	lua_setfield_function(L, "setPos", music_setpos);
	lua_setfield(L, -2, "Music");
	lua_pop(L, 2);
}
