//
// Copyright (c) 2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <malloc.h>
#ifdef NANOVG_GLEW
#  include <GL/glew.h>
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"
#include "demo.h"
#include "perf.h"


# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include <math.h>
# include <time.h>
# include "ausgabe.h"
# include "spiel.h"
# include "computerspieler.h"



#include "cJSON.h"
#include "cJSON_Utils.h"
#include "fastlz.h"
#include "lz4.h"
/* nuklear - 1.32.0 - public domain */
#define WIN32_LEAN_AND_MEAN
#include <string.h>
#include <time.h>
#include <limits.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_gdi.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define LIB_NAME "clua"

static void open_file(lua_State* L, const char* filename){
	if (luaL_dofile(L, filename) != LUA_TNIL){
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}
}

int l_system( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error(L, "expected 1 argument");
	}
	if ( lua_type( L, -1 ) != LUA_TSTRING )
	{
		return luaL_error( L, "argument 1 - this isn't a string" );
	}
	const char* str = lua_tostring( L, -1 );
	system( str );
	return 1;
}

int l_sleep( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error( L, "expected 1 argument" );
	}
	if ( lua_type( L, -1 ) != LUA_TNUMBER )
	{
		return luaL_error( L, "argument 1 - this isn't a number" );
	}
	const float time = lua_tonumber( L, -1 );
	Sleep( ( DWORD )time );
	return 1;
}

int l_setcolor( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error( L, "expected 1 argument" );
	}
	if ( lua_type( L, -1 ) != LUA_TNUMBER )
	{
		return luaL_error( L, "argument 1 - this isn't a number" );
	}
	const float num = lua_tonumber( L, -1 );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), (int)num );
	return 1;
}

int l_openfile( lua_State* L )
{
	if ( lua_gettop( L ) != 1 )
	{
		return luaL_error( L, "expected 1 argument" );
	}
	if ( lua_type( L, -1 ) != LUA_TSTRING )
	{
		return luaL_error( L, "argument 1 - this isn't a string" );
	}
	const char* filename = lua_tostring( L, -1 );
	FILE* handle;
	handle = fopen( filename, "r" );
	if (handle == NULL){
		//no such file!
		lua_getglobal(L, "print");
		lua_pushstring(L, "File reading error");
		lua_pcall(L, 1, 1, 0);
		return 1;
	}
	char buffer[1024];
	int i = 0;
	while (!feof(handle)){
		buffer[i] = (char)fgetc(handle);
		i++;
	}
	buffer[i] = '\0';
	lua_pushstring( L, buffer );
	fclose(handle);
	return 1;
}

int l_appendfile(lua_State* L){
	if (lua_gettop(L) != 2){
		return luaL_error(L, "expected 2 arguments");
	}
	if (lua_type(L, -1) != LUA_TSTRING){
		return luaL_error(L, "argument 2 - this isn't a string");
	}
	if (lua_type(L, -2) != LUA_TSTRING){
		return luaL_error(L, "argument 1 - this isn't a string");
	}
	const char* content = lua_tostring(L, -1);
	const char* filename = lua_tostring(L, -2);
	FILE* handle;
	handle = fopen(filename, "a");
	if (handle == NULL){
		lua_getglobal(L, "print");
		lua_pushstring(L, "File writing error");
		lua_pcall(L, 1, 1, 0);
		return 2;
	}
	fputs(content, handle);
	fclose(handle);
	return 2;
}

int l_writefile(lua_State* L){
	if (lua_gettop(L) != 2){
		return luaL_error(L, "expected 2 arguments");
	}
	if (lua_type(L, -1) != LUA_TSTRING){
		return luaL_error(L, "argument 2 - this isn't a string");
	}
	if (lua_type(L, -2) != LUA_TSTRING){
		return luaL_error(L, "argument 1 - this isn't a string");
	}
	const char* content = lua_tostring(L, -1);
	const char* filename = lua_tostring(L, -2);
	FILE* handle;
	handle = fopen(filename, "w");
	if (handle == NULL){
		lua_getglobal(L, "print");
		lua_pushstring(L, "File writing error");
		lua_pcall(L, 1, 1, 0);
		return 2;
	}
	fputs(content, handle);
	fclose(handle);
	return 2;
}

int l_include(lua_State* L){
	if (lua_gettop(L) != 1){
		return luaL_error(L, "expected 1 argument");
	}
	if (lua_type(L, -1) != LUA_TSTRING){
		return luaL_error(L, "argument 1 - this isn't a string");
	}
	const char* path = lua_tostring(L, -1);
	open_file(L, path);
	return 1;
}

static const struct luaL_Reg bindFuncs[] = {
	{"system", l_system },
	{"sleep", l_sleep },
	{"setcolor", l_setcolor },
	{"openfile", l_openfile },
	{"writefile", l_writefile},
	{"appendfile", l_appendfile},
	{"include", l_include},
	{NULL, NULL }
};

static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;

    return DefWindowProcW(wnd, msg, wparam, lparam);
}

void errorcb(int error, const char* desc)
{
	printf("GLFW error %d: %s\n", error, desc);
}

int blowup = 0;
int screenshot = 0;
int premult = 0;

static void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	NVG_NOTUSED(scancode);
	NVG_NOTUSED(mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		blowup = !blowup;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		screenshot = 1;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		premult = !premult;
}

int main()
{
 	lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    luaL_register( L, LIB_NAME, bindFuncs);
    open_file( L, "init.lua" );
    lua_close( L );
	getchar();


	enum farbe f;
	int spalte, zeile;
	enum richtung richtung;
	neues_spiel(NULL);
	zeichne_spielfeld();
	printf("\n\n V I E R   G E W I N N T\n"
		" =======================\n\n"
		" Zum Ziehen bitte die gew\201nschte Spalte anklicken!\n\n");
	f = rot;	// Rot f鋘gt an
	do {
		if (f == rot)
		{
			spalte = eingabe_spalte();
			if ((spalte < 0) || (spalte >= BREITE) || spalte_voll(NULL, spalte))
			{
				printf("\n r:Fehler (ung\201ltige Spalte %d)\n", spalte);
				continue;
			}
			printf(" r:%d", spalte);
		}
		else
		{
			spalte = computerspieler(f);
			printf(" g:%d", spalte);
			clear_mousebutton();
		}
		werfe_stein(spalte, spalte_anzahl(NULL, spalte), f);	// Animation
		setze_stein(NULL, spalte, f);
		if (f == rot)
			f = gelb;
		else
			f = rot;
	} while ((gewonnen(NULL) == keiner) && !alles_voll(NULL));

	switch (suche_4er(NULL, &spalte, &zeile, &richtung))
	{
	case rot: printf("\n\n Rot hat gewonnen!\n\n ");
		markiere_4er(spalte, zeile, richtung);
		meldung("Rot hat gewonnen!");
		break;
	case gelb: printf("\n\n Gelb hat gewonnen!\n\n ");
		markiere_4er(spalte, zeile, richtung);
		meldung("Gelb hat gewonnen!");
		break;
	default: printf("\n\n Unentschieden.\n\n ");
		meldung("Unentschieden!");
	}
	getchar();


    GdiFont* font;
    struct nk_context *ctx;

    WNDCLASSW wc;
    ATOM atom;
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;
    HWND wnd;
    HDC dc;
    int running = 1;
    int needs_refresh = 1;

    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"NuklearWindowClass";
    atom = RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Nuklear GDI Demo",
        style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);
    dc = GetDC(wnd);

    /* GUI */
    font = nk_gdifont_create("Arial", 14);
    ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (running)
    {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        if (needs_refresh == 0) {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                running = 0;
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            needs_refresh = 1;
        } else needs_refresh = 0;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            needs_refresh = 1;
        }
        nk_input_end(ctx);

        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 200),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;

            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");
            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
            nk_layout_row_dynamic(ctx, 22, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
        #ifdef INCLUDE_CALCULATOR
          calculator(ctx);
        #endif
        #ifdef INCLUDE_CANVAS
          canvas(ctx);
        #endif
        #ifdef INCLUDE_OVERVIEW
          overview(ctx);
        #endif
        #ifdef INCLUDE_NODE_EDITOR
          node_editor(ctx);
        #endif
        /* ----------------------------------------- */

        /* Draw */
        nk_gdi_render(nk_rgb(30,30,30));
    }

    nk_gdifont_del(font);
    ReleaseDC(wnd, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
	getchar();

	GLFWwindow* window;
	DemoData data;
	NVGcontext* vg = NULL;
	PerfGraph fps;
	double prevt = 0;

	if (!glfwInit()) {
		printf("Failed to init GLFW.");
		return -1;
	}

	initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");

	glfwSetErrorCallback(errorcb);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#ifdef DEMO_MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);
#endif

	window = glfwCreateWindow(1000, 600, "NanoVG", NULL, NULL);
//	window = glfwCreateWindow(1000, 600, "NanoVG", glfwGetPrimaryMonitor(), NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key);

	glfwMakeContextCurrent(window);
#ifdef NANOVG_GLEW
    if(glewInit() != GLEW_OK) {
		printf("Could not init glew.\n");
		return -1;
	}
#endif

#ifdef DEMO_MSAA
	vg = nvgCreateGL2(NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif
	if (vg == NULL) {
		printf("Could not init nanovg.\n");
		return -1;
	}

	if (loadDemoData(vg, &data) == -1)
		return -1;

	glfwSwapInterval(0);

	glfwSetTime(0);
	prevt = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		double mx, my, t, dt;
		int winWidth, winHeight;
		int fbWidth, fbHeight;
		float pxRatio;

		t = glfwGetTime();
		dt = t - prevt;
		prevt = t;
		updateGraph(&fps, dt);

		glfwGetCursorPos(window, &mx, &my);
		glfwGetWindowSize(window, &winWidth, &winHeight);
		glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

		// Calculate pixel ration for hi-dpi devices.
		pxRatio = (float)fbWidth / (float)winWidth;

		// Update and render
		glViewport(0, 0, fbWidth, fbHeight);
		if (premult)
			glClearColor(0,0,0,0);
		else
			glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

		renderDemo(vg, mx,my, winWidth,winHeight, t, blowup, &data);
		renderGraph(vg, 5,5, &fps);

		nvgEndFrame(vg);

		if (screenshot) {
			screenshot = 0;
			saveScreenShot(fbWidth, fbHeight, premult, "dump.png");
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	freeDemoData(vg, &data);

	nvgDeleteGL2(vg);

	glfwTerminate();
	return 0;
}
