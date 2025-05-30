#include "reilua_main.h"
#include "reilua_state.h"
#include "reilua_lua_core.h"
#include "reilua_textures.h"
#include "reilua_models.h"

State* state;

bool stateInit( int argn, const char** argc, const char* basePath ) {
	state = malloc( sizeof( State ) );

	state->basePath = malloc( STRING_LEN * sizeof( char ) );
	strncpy( state->basePath, basePath, STRING_LEN - 1 );

	state->hasWindow = true;
	state->run = true;
	state->resolution = (Vector2){ 800, 600 };
	state->luaState = NULL;
	state->logLevelInvalid = LOG_ERROR;
	state->gcUnload = true;
	state->lineSpacing = 15;
	state->mouseOffset = (Vector2){ 0, 0 };
	state->mouseScale = (Vector2){ 1, 1 };

	InitWindow( state->resolution.x, state->resolution.y, "ReiLua" );

	if ( !IsWindowReady() ) {
		state->hasWindow = false;
		state->run = false;
	}
	if ( state->run ) {
		state->run = luaInit( argn, argc );
	}
	state->defaultFont = GetFontDefault();
	state->guiFont = GuiGetFont();
	state->defaultMaterial = LoadMaterialDefault();
	state->defaultTexture = (Texture){ 1, 1, 1, 1, 7 };
	state->RLGLcurrentShaderLocs = malloc( RL_MAX_SHADER_LOCATIONS * sizeof( int ) );
	int* defaultShaderLocs = rlGetShaderLocsDefault();

	for ( int i = 0; i < RL_MAX_SHADER_LOCATIONS; i++ ) {
		state->RLGLcurrentShaderLocs[i] = defaultShaderLocs[i];
	}
#ifdef PLATFORM_DESKTOP_SDL
	state->SDL_eventQueue = malloc( PLATFORM_SDL_EVENT_QUEUE_LEN * sizeof( SDL_Event ) );
	state->SDL_eventQueueLen = 0;
#endif

	return state->run;
}

void stateInitInterpret( int argn, const char** argc ) {
	state = malloc( sizeof( State ) );
	luaInit( argn, argc );
}

void stateFree() {
	if ( IsAudioDeviceReady() ) {
		CloseAudioDevice();
	}
	if ( state->luaState != NULL ) {
		lua_close( state->luaState );
		state->luaState = NULL;
	}
	if ( state->hasWindow ) {
		CloseWindow();
	}
#ifdef PLATFORM_DESKTOP_SDL
	free( state->SDL_eventQueue );
#endif
	free( state->basePath );
	free( state->RLGLcurrentShaderLocs );
	free( state );
}
