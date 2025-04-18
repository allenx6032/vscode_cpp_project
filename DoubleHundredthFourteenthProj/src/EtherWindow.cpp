#include "EtherWindow.h"

std::unordered_map<const char*, EtherWindow*> mapAllWindows;

ModuleWindow::ModuleWindow()
{
	_vCMethods =
	{
		{"CreateWindow", CreateWindow}
	};

	_vMacros =
	{
		{"WINDOW_POSITION_DEFAULT", (int)WINDOW::POSITION_DEFAULT},

		{"WINDOW_FULLSCREEN", (int)WINDOW::FULLSCREEN},
		{"WINDOW_FULLSCREEN_DESKTOP", (int)WINDOW::FULLSCREEN_DESKTOP},
		{"WINDOW_BORDERLESS", (int)WINDOW::BORDERLESS},
		{"WINDOW_RESIZABLE", (int)WINDOW::RESIZABLE},
		{"WINDOW_MAXIMIZED", (int)WINDOW::MAXIMIZED},
		{"WINDOW_MINMIZED", (int)WINDOW::MINIMIZED},

		{"WINDOW_MODE_WINDOWED", (int)WINDOW::MODE_WINDOWED},
		{"WINDOW_MODE_FULLSCREEN", (int)WINDOW::MODE_FULLSCREEN},
		{"WINDOW_MODE_FULLSCREEN_DESKTOP", (int)WINDOW::MODE_FULLSCREEN_DESKTOP},
		{"WINDOW_MODE_BORDERLESS", (int)WINDOW::MODE_BORDERLESS},
		{"WINDOW_MODE_BORDERED", (int)WINDOW::MODE_BORDERED},
		{"WINDOW_MODE_RESIZABLE", (int)WINDOW::MODE_RESIZABLE},
		{"WINDOW_MODE_SIZEFIXED", (int)WINDOW::MODE_SIZEFIXED}
	};

	_vMetaData =
	{
		{
			"EtherWindow",
			nullptr,
			{
				{"CloseWindow", window_CloseWindow},
				{"GetWindowTitle", window_GetWindowTitle},
				{"SetWindowTitle", window_SetWindowTitle},
				{"SetWindowMode", window_SetWindowMode},
				{"SetWindowOpacity", window_SetWindowOpacity},
				{"GetWindowSize", window_GetWindowSize},
				{"SetWindowSize", window_SetWindowSize},
				{"GetWindowMaxSize", window_GetWindowMaxSize},
				{"SetWindowMaxSize", window_SetWindowMaxSize},
				{"GetWindowMinSize", window_GetWindowMinSize},
				{"SetWindowMinSize", window_SetWindowMinSize},
				{"GetWindowPosition", window_GetWindowPosition},
				{"SetWindowPosition", window_SetWindowPosition},
				{"GetWindowID", window_GetWindowID},
				{"AddNode", window_AddNode},
				{"DeleteNode", window_DeleteNode}
			},
			__gc_Window
		}
	};
}

ModuleWindow& ModuleWindow::Instance()
{
	static ModuleWindow* _instance = new ModuleWindow();
	return *_instance;
}

ETHER_API CreateWindow(lua_State* L)
{
	SDL_Rect rect = GetRectParam(L, 2);

	rect.x = rect.x == (int)WINDOW::POSITION_DEFAULT ? SDL_WINDOWPOS_UNDEFINED : rect.x;
	rect.y = rect.y == (int)WINDOW::POSITION_DEFAULT ? SDL_WINDOWPOS_UNDEFINED : rect.y;

	int flags = SDL_WINDOW_SHOWN;
	luaL_argcheck(L, lua_istable(L, 3), 3, "table expected");
	lua_pushnil(L);
	while (lua_next(L, 3))
	{
		lua_pushvalue(L, -2);
		switch ((int)lua_tonumber(L, -2))
		{
		case (int)WINDOW::FULLSCREEN:
			flags |= SDL_WINDOW_FULLSCREEN;
			break;
		case (int)WINDOW::FULLSCREEN_DESKTOP:
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			break;
		case (int)WINDOW::BORDERLESS:
			flags |= SDL_WINDOW_BORDERLESS;
			break;
		case (int)WINDOW::RESIZABLE:
			flags |= SDL_WINDOW_RESIZABLE;
			break;
		case (int)WINDOW::MAXIMIZED:
			flags |= SDL_WINDOW_MAXIMIZED;
			break;
		case (int)WINDOW::MINIMIZED:
			flags |= SDL_WINDOW_MINIMIZED;
			break;
		default:
			luaL_error(L, "bad argument #3 to 'CreateWindow' (the elements of table must be MACRO number, got %s)", luaL_typename(L, -2));
			break;
		}
		lua_pop(L, 2);
	}

	//创建一个Window
	const char* strName = luaL_checkstring(L, 1);
	EtherWindow* pEWindow = new EtherWindow(SDL_CreateWindow(strName, rect.x, rect.y, rect.w, rect.h, flags));
	pEWindow->name = strName;
	pEWindow->pRenderer = SDL_CreateRenderer(pEWindow->pWindow, -1, SDL_RENDERER_ACCELERATED);

	//给全局窗口表推送一个
	mapAllWindows[strName] = pEWindow;

	//给lua虚拟机推送一个
	EtherWindow** uppWindow = (EtherWindow**)lua_newuserdata(L, sizeof(EtherWindow*));
	*uppWindow = pEWindow;
	luaL_getmetatable(L, "EtherWindow");
	lua_setmetatable(L, -2);

	return 1;
}

ETHER_API window_CloseWindow(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	SDL_DestroyWindow(pEWindow->pWindow);

	mapAllWindows.erase(mapAllWindows.find(pEWindow->name));

	delete pEWindow;

	return 0;
}

ETHER_API window_GetWindowTitle(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	lua_pushstring(L, pEWindow->name);

	return 1;
}

ETHER_API window_SetWindowTitle(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	pEWindow->name = luaL_checkstring(L, 2);
	SDL_SetWindowTitle(pEWindow->pWindow, pEWindow->name);

	return 0;
}

ETHER_API window_SetWindowMode(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));

	switch ((int)luaL_checknumber(L, 2))
	{
	case (int)WINDOW::MODE_WINDOWED:
		SDL_SetWindowFullscreen(pEWindow->pWindow, 0);
		break;
	case (int)WINDOW::MODE_FULLSCREEN:
		SDL_SetWindowFullscreen(pEWindow->pWindow, SDL_WINDOW_FULLSCREEN);
		break;
	case (int)WINDOW::MODE_FULLSCREEN_DESKTOP:
		SDL_SetWindowFullscreen(pEWindow->pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	case (int)WINDOW::MODE_BORDERLESS:
		SDL_SetWindowBordered(pEWindow->pWindow, SDL_FALSE);
		break;
	case (int)WINDOW::MODE_BORDERED:
		SDL_SetWindowFullscreen(pEWindow->pWindow, SDL_TRUE);
		break;
	case (int)WINDOW::MODE_RESIZABLE:
		SDL_SetWindowResizable(pEWindow->pWindow, SDL_TRUE);
		break;
	case (int)WINDOW::MODE_SIZEFIXED:
		SDL_SetWindowResizable(pEWindow->pWindow, SDL_FALSE);
		break;
	default:
		luaL_error(L, "bad argument #1 to 'SetWindowMode' (number-MACRO expected, got %s)", luaL_typename(L, -1));
		break;
	}

	return 0;
}

ETHER_API window_SetWindowOpacity(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	SDL_SetWindowOpacity(pEWindow->pWindow, luaL_checknumber(L, 2));

	return 0;
}

ETHER_API window_GetWindowSize(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));

	int width = 0, height = 0;
	SDL_GetWindowSize(pEWindow->pWindow, &width, &height);
	lua_pushnumber(L, width);
	lua_pushnumber(L, height);

	return 2;
}

ETHER_API window_SetWindowSize(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	SDL_SetWindowSize(pEWindow->pWindow, luaL_checknumber(L, 2), luaL_checknumber(L, 3));

	return 0;
}

ETHER_API window_GetWindowMaxSize(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	int width = 0, height = 0;
	SDL_GetWindowMaximumSize(pEWindow->pWindow, &width, &height);
	lua_pushnumber(L, width);
	lua_pushnumber(L, height);

	return 2;
}

ETHER_API window_SetWindowMaxSize(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	SDL_SetWindowMaximumSize(pEWindow->pWindow, luaL_checknumber(L, 2), luaL_checknumber(L, 3));

	return 0;
}

ETHER_API window_GetWindowMinSize(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	int width = 0, height = 0;
	SDL_GetWindowMinimumSize(pEWindow->pWindow, &width, &height);
	lua_pushnumber(L, width);
	lua_pushnumber(L, height);

	return 2;
}

ETHER_API window_SetWindowMinSize(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	SDL_SetWindowMinimumSize(pEWindow->pWindow, luaL_checknumber(L, 2), luaL_checknumber(L, 3));

	return 0;
}

ETHER_API window_GetWindowPosition(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	SDL_Point point;
	SDL_GetWindowPosition(pEWindow->pWindow, &point.x, &point.y);
	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushnumber(L, point.x);
	lua_settable(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, point.y);
	lua_settable(L, -3);

	return 1;
}

ETHER_API window_SetWindowPosition(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	SDL_Point point =  GetPointParam(L, 2);
	SDL_SetWindowPosition(pEWindow->pWindow, point.x, point.y);

	return 0;
}

ETHER_API window_GetWindowID(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	lua_pushnumber(L, SDL_GetWindowID(pEWindow->pWindow));

	return 1;
}

ETHER_API window_AddNode(lua_State* L)
{
	using namespace std;
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	EtherNode* pNode = (EtherNode*)(*(void**)lua_touserdata(L, 2));

	pNode->pRenderer = pEWindow->pRenderer;
	pEWindow->vNode.push_back(pNode);

	return 0;
}

ETHER_API window_DeleteNode(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	int index = lua_tonumber(L, 2) - 1;

	auto iter = pEWindow->vNode.begin() + index;
	pEWindow->vNode.erase(iter);

	return 0;
}

ETHER_API __gc_Window(lua_State* L)
{
	EtherWindow* pEWindow = (EtherWindow*)(*(void**)luaL_checkudata(L, 1, "EtherWindow"));
	delete pEWindow;

	return 0;
}