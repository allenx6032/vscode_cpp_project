#ifndef _MAIN_H_
#define _MAIN_H_

#include "EtherEngine_Macros.h"
#include "EtherEngine_ModuleWindow.h"
#include "EtherEngine_ModuleGraphic.h"
#include "EtherEngine_ModuleMedia.h"
#include "EtherEngine_ModuleInteractivity.h"
#include "EtherEngine_ModuleTime.h"
#include "EtherEngine_ModuleOS.h"
#include "EtherEngine_ModuleAlgorithm.h"
#include "EtherEngine_ModuleNetwork.h"
#include "EtherEngine_ModuleString.h"
#include "EtherEngine_ModuleJSON.h"
#include "EtherEngine_ModuleCompress.h"
#include "EtherEngine_ModuleXML.h"

#include "EtherEngine_lua.hpp"
#include <cJSON.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <unordered_map>

#ifndef _ETHER_DEBUG_
#ifdef __WINDOWS__
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#else
/* 以图形应用程序模式编译 */
#endif
#endif

#define MODULENAME_ALGORITHM		"Algorithm"
#define MODULENAME_GRAPHIC			"Graphic"
#define MODULENAME_INTERACTIVITY	"Interactivity"
#define MODULENAME_MEDIA			"Media"
#define MODULENAME_NETWORK			"Network"
#define MODULENAME_OS				"OS"
#define MODULENAME_TIME				"Time"
#define MODULENAME_WINDOW			"Window"
#define MODULENAME_STRING			"String"
#define MODULENAME_JSON				"JSON"
#define MODULENAME_COMPRESS			"Compress"
#define MODULENAME_XML				"XML"

#define DEFAULTNAME_ENTRY			"Main.lua"

#define CONFIGSUCCESS				0
#define CONFIGERROR_LOADING			-1
#define CONFIGERROR_PARSING			-2

#ifndef __WINDOWS__
extern char** environ;
#endif

int _LoadConfig();

void _PushArgs(lua_State* L, int argc, char** argv, char** envp);

void _HandleQuit();

ETHER_API usingModule(lua_State* L);

ETHER_API getVersion(lua_State* L);

#endif // !_MAIN_H_