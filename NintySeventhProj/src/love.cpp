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

#include "love_version.h"
#include "love_love.h"
#include <SDL.h>

#ifdef LOVE_BUILD_EXE

// Lua
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include "love_runtime.h"

#ifdef LOVE_WINDOWS
#include <windows.h>
#include <shellapi.h>
#endif // LOVE_WINDOWS

#ifdef LOVE_MACOSX
#include "OSX.h"
#endif // LOVE_MACOSX

#ifdef LOVE_WINDOWS
extern "C"
{
// Prefer the higher performance GPU on Windows systems that use nvidia Optimus.
// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
// TODO: Re-evaluate in the future when the average integrated GPU in Optimus
// systems is less mediocre?
LOVE_EXPORT DWORD NvOptimusEnablement = 0x00000001;
}
#endif



#ifdef LOVE_LEGENDARY_CONSOLE_IO_HACK
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#endif // LOVE_LEGENDARY_CONSOLE_IO_HACK

// Libraries.
#ifdef LOVE_ENABLE_LUASOCKET
#	include "luasocket.h"
#endif
#ifdef LOVE_ENABLE_ENET
#	include "luaenet.h"
#endif
#ifdef LOVE_ENABLE_LUAUTF8
#	include "luautf8lib.h"
#endif

// Scripts
#include "love_boot.lua.h"

// All modules define a c-accessible luaopen
// so let's make use of those, instead
// of addressing implementations directly.
extern "C"
{
#if defined(LOVE_ENABLE_AUDIO)
	extern int luaopen_love_audio(lua_State*);
#endif
#if defined(LOVE_ENABLE_EVENT)
	extern int luaopen_love_event(lua_State*);
#endif
#if defined(LOVE_ENABLE_FILESYSTEM)
	extern int luaopen_love_filesystem(lua_State*);
#endif
#if defined(LOVE_ENABLE_FONT)
	extern int luaopen_love_font(lua_State*);
#endif
#if defined(LOVE_ENABLE_GRAPHICS)
	extern int luaopen_love_graphics(lua_State*);
#endif
#if defined(LOVE_ENABLE_IMAGE)
	extern int luaopen_love_image(lua_State*);
#endif
#if defined(LOVE_ENABLE_JOYSTICK)
	extern int luaopen_love_joystick(lua_State*);
#endif
#if defined(LOVE_ENABLE_KEYBOARD)
	extern int luaopen_love_keyboard(lua_State*);
#endif
#if defined(LOVE_ENABLE_MATH)
	extern int luaopen_love_math(lua_State*);
#endif
#if defined(LOVE_ENABLE_MOUSE)
	extern int luaopen_love_mouse(lua_State*);
#endif
#if defined(LOVE_ENABLE_PHYSICS)
	extern int luaopen_love_physics(lua_State*);
#endif
#if defined(LOVE_ENABLE_SOUND)
	extern int luaopen_love_sound(lua_State*);
#endif
#if defined(LOVE_ENABLE_SYSTEM)
	extern int luaopen_love_system(lua_State*);
#endif
#if defined(LOVE_ENABLE_TIMER)
	extern int luaopen_love_timer(lua_State*);
#endif
#if defined(LOVE_ENABLE_THREAD)
	extern int luaopen_love_thread(lua_State*);
#endif
#if defined(LOVE_ENABLE_WINDOW)
	extern int luaopen_love_window(lua_State*);
#endif
	extern int luaopen_love_boot(lua_State*);
}

static const luaL_Reg modules[] = {
#if defined(LOVE_ENABLE_AUDIO)
	{ "love.audio", luaopen_love_audio },
#endif
#if defined(LOVE_ENABLE_EVENT)
	{ "love.event", luaopen_love_event },
#endif
#if defined(LOVE_ENABLE_FILESYSTEM)
	{ "love.filesystem", luaopen_love_filesystem },
#endif
#if defined(LOVE_ENABLE_FONT)
	{ "love.font", luaopen_love_font },
#endif
#if defined(LOVE_ENABLE_GRAPHICS)
	{ "love.graphics", luaopen_love_graphics },
#endif
#if defined(LOVE_ENABLE_IMAGE)
	{ "love.image", luaopen_love_image },
#endif
#if defined(LOVE_ENABLE_JOYSTICK)
	{ "love.joystick", luaopen_love_joystick },
#endif
#if defined(LOVE_ENABLE_KEYBOARD)
	{ "love.keyboard", luaopen_love_keyboard },
#endif
#if defined(LOVE_ENABLE_MATH)
	{ "love.math", luaopen_love_math },
#endif
#if defined(LOVE_ENABLE_MOUSE)
	{ "love.mouse", luaopen_love_mouse },
#endif
#if defined(LOVE_ENABLE_PHYSICS)
	{ "love.physics", luaopen_love_physics },
#endif
#if defined(LOVE_ENABLE_SOUND)
	{ "love.sound", luaopen_love_sound },
#endif
#if defined(LOVE_ENABLE_SYSTEM)
	{ "love.system", luaopen_love_system },
#endif
#if defined(LOVE_ENABLE_TIMER)
	{ "love.timer", luaopen_love_timer },
#endif
#if defined(LOVE_ENABLE_THREAD)
	{ "love.thread", luaopen_love_thread },
#endif
#if defined(LOVE_ENABLE_WINDOW)
	{ "love.window", luaopen_love_window },
#endif
	{ "love.boot", luaopen_love_boot },
	{ 0, 0 }
};

#ifdef LOVE_LEGENDARY_CONSOLE_IO_HACK
int w__openConsole(lua_State *L);
#endif // LOVE_LEGENDARY_CONSOLE_IO_HACK

const char *love_version()
{
	// Do not refer to love::VERSION here, the linker
	// will patch it back up to the executable's one..
	return LOVE_VERSION_STRING;
}

const char *love_codename()
{
	return love::VERSION_CODENAME;
}

static int w_love_getVersion(lua_State *L)
{
	lua_pushinteger(L, love::VERSION_MAJOR);
	lua_pushinteger(L, love::VERSION_MINOR);
	lua_pushinteger(L, love::VERSION_REV);
	lua_pushstring(L, love::VERSION_CODENAME);
	return 4;
}

int luaopen_love(lua_State * L)
{
	love::luax_insistglobal(L, "love");

	// Set version information.
	lua_pushstring(L, love::VERSION);
	lua_setfield(L, -2, "_version");

	lua_pushnumber(L, love::VERSION_MAJOR);
	lua_setfield(L, -2, "_version_major");
	lua_pushnumber(L, love::VERSION_MINOR);
	lua_setfield(L, -2, "_version_minor");
	lua_pushnumber(L, love::VERSION_REV);
	lua_setfield(L, -2, "_version_revision");

	lua_pushstring(L, love::VERSION_CODENAME);
	lua_setfield(L, -2, "_version_codename");

#ifdef LOVE_LEGENDARY_CONSOLE_IO_HACK
	lua_pushcfunction(L, w__openConsole);
	lua_setfield(L, -2, "_openConsole");
#endif // LOVE_LEGENDARY_CONSOLE_IO_HACK

	lua_newtable(L);

	for (int i = 0; love::VERSION_COMPATIBILITY[i] != 0; ++i)
	{
		lua_pushstring(L, love::VERSION_COMPATIBILITY[i]);
		lua_rawseti(L, -2, i+1);
	}

	lua_setfield(L, -2, "_version_compat");

	lua_pushcfunction(L, w_love_getVersion);
	lua_setfield(L, -2, "getVersion");

#ifdef LOVE_WINDOWS
	lua_pushstring(L, "Windows");
#elif defined(LOVE_MACOSX)
	lua_pushstring(L, "OS X");
#elif defined(LOVE_LINUX)
	lua_pushstring(L, "Linux");
#else
	lua_pushstring(L, "Unknown");
#endif
	lua_setfield(L, -2, "_os");

	// Preload module loaders.
	for (int i = 0; modules[i].name != 0; i++)
		love::luax_preload(L, modules[i].func, modules[i].name);

#ifdef LOVE_ENABLE_LUASOCKET
	love::luasocket::__open(L);
#endif
#ifdef LOVE_ENABLE_ENET
	love::luax_preload(L, luaopen_enet, "enet");
#endif
#ifdef LOVE_ENABLE_LUAUTF8
	love::luax_preload(L, luaopen_luautf8, "utf8");
#endif

	return 1;
}

#ifdef LOVE_LEGENDARY_CONSOLE_IO_HACK

// Mostly taken from the Windows 8.1 SDK's VersionHelpers.h.
static bool IsWindowsVistaOrGreater()
{
	OSVERSIONINFOEXW osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0};

	osvi.dwMajorVersion = HIBYTE(_WIN32_WINNT_VISTA);
	osvi.dwMinorVersion = LOBYTE(_WIN32_WINNT_VISTA);
	osvi.wServicePackMajor = 0;

	DWORDLONG majorversionmask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
	DWORDLONG versionmask = VerSetConditionMask(majorversionmask, VER_MINORVERSION, VER_GREATER_EQUAL);
	DWORDLONG mask = VerSetConditionMask(versionmask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, mask) != FALSE;
}

int w__openConsole(lua_State *L)
{
	static bool is_open = false;

	if (is_open)
	{
		love::luax_pushboolean(L, is_open);
		return 1;
	}

	is_open = true;

	// FIXME: we don't call AttachConsole in Windows XP because it seems to
	// break later AllocConsole calls if it fails. A better workaround might be
	// possible, but it's hard to find a WinXP system to test on these days...
	if (!IsWindowsVistaOrGreater() || !AttachConsole(ATTACH_PARENT_PROCESS))
	{
		// Create our own console if we can't attach to an existing one.
		if (!AllocConsole())
		{
			is_open = false;
			love::luax_pushboolean(L, is_open);
			return 1;
		}

		SetConsoleTitle(TEXT("LOVE Console"));

		const int MAX_CONSOLE_LINES = 5000;
		CONSOLE_SCREEN_BUFFER_INFO console_info;

		// Set size.
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_info);
		console_info.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), console_info.dwSize);
	}

	FILE *fp;

	// Redirect stdout.
	fp = freopen("CONOUT$", "w", stdout);
	if (L && fp == NULL)
		return luaL_error(L, "Console redirection of stdout failed.");

	// Redirect stdin.
	fp = freopen("CONIN$", "r", stdin);
	if (L && fp == NULL)
		return luaL_error(L, "Console redirection of stdin failed.");

	// Redirect stderr.
	fp = freopen("CONOUT$", "w", stderr);
	if (L && fp == NULL)
		return luaL_error(L, "Console redirection of stderr failed.");

	love::luax_pushboolean(L, is_open);
	return 1;
}

#endif // LOVE_LEGENDARY_CONSOLE_IO_HACK

int luaopen_love_boot(lua_State *L)
{
	if (luaL_loadbuffer(L, (const char *)love::boot_lua, sizeof(love::boot_lua), "love_boot.lua") == 0)
		lua_call(L, 0, 1);

	return 1;
}

#ifdef LOVE_LEGENDARY_UTF8_ARGV_HACK

void get_utf8_arguments(int &argc, char **&argv)
{
	LPWSTR cmd = GetCommandLineW();

	if (!cmd)
		return;

	LPWSTR *argv_w = CommandLineToArgvW(cmd, &argc);

	argv = new char *[argc];

	for (int i = 0; i < argc; ++i)
	{
		// Size of wide char buffer (plus one for trailing '\0').
		size_t wide_len = wcslen(argv_w[i]) + 1;

		// Get size in UTF-8.
		int utf8_size = WideCharToMultiByte(CP_UTF8, 0, argv_w[i], wide_len, argv[i], 0, 0, 0);

		argv[i] = new char[utf8_size];

		// Convert to UTF-8.
		int ok = WideCharToMultiByte(CP_UTF8, 0, argv_w[i], wide_len, argv[i], utf8_size, 0, 0);

		int len = strlen(argv[i]);

		if (!ok)
			printf("Warning: could not convert to UTF8.\n");
	}

	LocalFree(argv_w);
}

#endif // LOVE_LEGENDARY_UTF8_ARGV_HACK

#ifdef LOVE_LEGENDARY_APP_ARGV_HACK

#include <vector>

static void get_app_arguments(int argc, char **argv, int &new_argc, char **&new_argv)
{
	std::vector<std::string> temp_argv;
	for (int i = 0; i < argc; i++)
	{
		// Don't copy -psn_xxx argument from argv.
		if (i == 0 || strncmp(argv[i], "-psn_", 5) != 0)
			temp_argv.push_back(std::string(argv[i]));
	}

	// Check for a drop file string.
	std::string dropfilestr = love::osx::checkDropEvents();
	if (!dropfilestr.empty())
	{
		temp_argv.insert(temp_argv.begin() + 1, dropfilestr);
	}
	else
	{
		// If it exists, add the love file in love.app/Contents/Resources/ to argv.
		std::string loveResourcesPath = love::osx::getLoveInResources();
		if (!loveResourcesPath.empty())
		{
			// Run in pseudo-fused mode.
			std::vector<std::string>::iterator it = temp_argv.begin();
			it = temp_argv.insert(it + 1, loveResourcesPath);
			temp_argv.insert(it + 1, std::string("--fused"));
		}
	}

	// Copy temp argv vector to new argv array.
	new_argc = (int) temp_argv.size();
	new_argv = new char *[new_argc+1];

	for (int i = 0; i < new_argc; i++)
	{
		new_argv[i] = new char[temp_argv[i].length() + 1];
		strcpy(new_argv[i], temp_argv[i].c_str());
	}

	new_argv[new_argc] = NULL;
}

#endif // LOVE_LEGENDARY_APP_ARGV_HACK

static int love_preload(lua_State *L, lua_CFunction f, const char *name)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");
	lua_pushcfunction(L, f);
	lua_setfield(L, -2, name);
	lua_pop(L, 2);
	return 0;
}

#undef main
int main(int argc, char **argv)
{
	printf("LOVE %s (%s)\n", love_version(), love_codename());
#ifdef LOVE_LEGENDARY_UTF8_ARGV_HACK
	int hack_argc = 0;	char **hack_argv = 0;
	get_utf8_arguments(hack_argc, hack_argv);
	argc = hack_argc;
	argv = hack_argv;
#endif // LOVE_LEGENDARY_UTF8_ARGV_HACK

#ifdef LOVE_LEGENDARY_APP_ARGV_HACK
	int hack_argc = 0;
	char **hack_argv = 0;
	get_app_arguments(argc, argv, hack_argc, hack_argv);
	argc = hack_argc;
	argv = hack_argv;
#endif // LOVE_LEGENDARY_APP_ARGV_HACK

	if (strcmp(LOVE_VERSION_STRING, love_version()) != 0)
	{
		printf("Version mismatch detected!\nLOVE binary is version %s\n"
				"LOVE library is version %s\n", LOVE_VERSION_STRING, love_version());
		return 1;
	}

	// Oh, you just want the version? Okay!
	if (argc > 1 && strcmp(argv[1], "--version") == 0)
	{
		printf("LOVE %s (%s)\n", love_version(), love_codename());
		return 0;
	}

	// Create the virtual machine.
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	// Add love to package.preload for easy requiring.
	love_preload(L, luaopen_love, "love");

	// Add command line arguments to global arg (like stand-alone Lua).
	{
		lua_newtable(L);

		if (argc > 0)
		{
			lua_pushstring(L, argv[0]);
			lua_rawseti(L, -2, -2);
		}

		lua_pushstring(L, "embedded boot.lua");
		lua_rawseti(L, -2, -1);

		for (int i = 1; i < argc; i++)
		{
			lua_pushstring(L, argv[i]);
			lua_rawseti(L, -2, i);
		}

		lua_setglobal(L, "arg");
	}

	// require "love"
	lua_getglobal(L, "require");
	lua_pushstring(L, "love");
	lua_call(L, 1, 1); // leave the returned table on the stack.

	// Add love._exe = true.
	// This indicates that we're running the standalone version of love, and not
	// the library version.
	{
		lua_pushboolean(L, 1);
		lua_setfield(L, -2, "_exe");
	}

	// Pop the love table returned by require "love".
	lua_pop(L, 1);

	// require "love.boot" (preloaded when love was required.)
	lua_getglobal(L, "require");
	lua_pushstring(L, "love.boot");
	lua_call(L, 1, 1);

	// Call the returned boot function.
	lua_call(L, 0, 1);

	int retval = 0;
	if (lua_isnumber(L, -1))
		retval = (int) lua_tonumber(L, -1);

	lua_close(L);

#if defined(LOVE_LEGENDARY_UTF8_ARGV_HACK) || defined(LOVE_LEGENDARY_APP_ARGV_HACK)
	if (hack_argv)
	{
		for (int i = 0; i<hack_argc; ++i)
			delete [] hack_argv[i];
		delete [] hack_argv;
	}
#endif // LOVE_LEGENDARY_UTF8_ARGV_HACK || LOVE_LEGENDARY_APP_ARGV_HACK
	return retval;
}

#endif // LOVE_BUILD_EXE
