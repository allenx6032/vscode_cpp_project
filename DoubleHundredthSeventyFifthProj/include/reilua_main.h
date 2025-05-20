#pragma once

#define STRING_LEN 1024

#define VERSION_MAJOR 0
#define VERSION_MINOR 8
#define VERSION_PATCH 0
#define VERSION_DEV 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "glad.h"

#ifdef PLATFORM_DESKTOP
	#include "GLFW/glfw3.h"
	#include "GLFW/glfw3native.h"
#elif PLATFORM_DESKTOP_SDL
	#include <SDL.h>
#endif

#ifdef LUAJIT
	#ifdef SHARED
		#include <lua.h>
		#include <luajit.h>
		#include <lualib.h>
		#include <lauxlib.h>
	#else
		#include "luajit/lua.h"
		#include "luajit/luajit.h"
		#include "luajit/lualib.h"
		#include "luajit/lauxlib.h"
	#endif
#else
	#ifdef SHARED
		#include <lua.h>
		#include <lualib.h>
		#include <lauxlib.h>
	#else
		#include "lua_includes.h"
	#endif
#endif

#ifdef SHARED
	#include <raylib.h>
	#include <rlgl.h>
	#include <raymath.h>
	#include <raygui.h>
	#include <rlights.h>
	#include <rcamera.h>
#else
	#include "reilua_raylib.h"
	#include "reilua_rlgl.h"
	#include "reilua_raymath.h"
	#include "reilua_raygui.h"
	#include "reilua_rlights.h"
	#include "reilua_rcamera.h"

	// #include "raylib.h"
	// #include "raylib_raudio.h"
	// #include "raylib_rlgl.h"
	// #include "raylib_rmath.h"
	// #include "raylib_rgui.h"
	// #include "raylib_rlights.h"
	// #include "raylib_rcamera.h"
	

	// #include "reilua_raylib.h"
	// #include "reilua_core.h"
	// #include "reilua_lua_core.h"
	// #include "reilua_audio.h"
	// #include "reilua_rlgl.h"
	// #include "reilua_rmath.h"
	// #include "reilua_rgui.h"
	// #include "reilua_rlights.h"
	// #include "reilua_rcamera.h"
#endif

