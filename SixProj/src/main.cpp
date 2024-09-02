#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <tchar.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include "glad/glad.h"

#include "GL/glew.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

//.h文件结束的地方
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */ 

// include nuklear.h before the sokol_nuklear.h implementation
// #define NK_INCLUDE_FIXED_TYPES
// #define NK_INCLUDE_STANDARD_IO
// #define NK_INCLUDE_DEFAULT_ALLOCATOR
// #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
// #define NK_INCLUDE_FONT_BAKING
// #define NK_INCLUDE_DEFAULT_FONT
// #define NK_INCLUDE_STANDARD_VARARGS
// #define NK_IMPLEMENTATION
// #define NK_GDI_IMPLEMENTATION
// #include "nuklear.h"
// #include "nuklear_gdi.h"
// #include <time.h>
// #include <limits.h>
// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600

// #ifdef INCLUDE_ALL
//   #define INCLUDE_STYLE
//   #define INCLUDE_CALCULATOR
//   #define INCLUDE_CANVAS
//   #define INCLUDE_OVERVIEW
//   #define INCLUDE_NODE_EDITOR
// #endif

// #ifdef INCLUDE_STYLE
//   #include "nk_style.c"
// #endif
// #ifdef INCLUDE_CALCULATOR
//   #include "nk_calculator.c"
// #endif
// #ifdef INCLUDE_CANVAS
//   #include "nk_canvas.c"
// #endif
// #ifdef INCLUDE_OVERVIEW
//   #include "nk_overview.c"
// #endif
// #ifdef INCLUDE_NODE_EDITOR
//   #include "nk_node_editor.c"
// #endif

// /* ===============================================================
//  *
//  *                          DEMO
//  *
//  * ===============================================================*/
// static LRESULT CALLBACK
// WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
// {
//     switch (msg)
//     {
//     case WM_DESTROY:
//         PostQuitMessage(0);
//         return 0;
//     }
//     if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
//         return 0;
//     return DefWindowProcW(wnd, msg, wparam, lparam);
// }

#include "bp.h"
typedef struct 
{
    /* resource */
    short res;
    /* consumer coroutine breakpoint */
    unsigned char consumer_bp;
    /* produce coroutine breakpoint */
    unsigned char producer_bp;
} res_t;

#define TRUE    1
#define FALSE   (!TRUE)

int consumer(res_t *res)
{
    /* bpd coroutine default breakpoint pointer */
    unsigned char *bpd = &res->consumer_bp;
    /* coroutine begin */
    bpd_begin(2);
    while (1)
    {
        /* wait number of resource > 0 */
        while (res->res <= 0)
        {
            bpd_yield(1) FALSE;
        }
        /* consume */
        res->res--;
        printf("consume a resource, number of res:%d\n", res->res);
        /* wait next consume */
        bpd_yield(2) TRUE;
    }
    /* coroutine end */
    bpd_end();
}

int producer(res_t *res)
{
    /* bpd coroutine default breakpoint pointer */
    unsigned char *bpd = &res->producer_bp;
    /* coroutine begin */
    bpd_begin(2);
    while (1)
    {
        /* wait number of resource < 30 */
        while (res->res >= 30)
        {
            bpd_yield(1) FALSE;
        }
        /* produce */
        res->res++;
        printf("produce a resource, number of res:%d\n", res->res);
        /* wait next produce */
        bpd_yield(2) TRUE;
    }
    /* coroutine end */
    bpd_end();
}

#include "LightHook.h"
static HookInformation testHook;
typedef int(*TestFunction_t)(int param1, int param2);
__declspec(noinline) int TestFunction(int param1, int param2)
{
    param2 -= param1;
    param1 += param2;
    for (int i = 0; i < 10; i++)
        param1 += param2 * param2;
    param2 *= param1;
    param1 *= param2;
    for (int i = 0; i < 5; i++)
        param1 += param2 * param2;
    printf("in func: %i\n", param1);
    return param1;
}

__declspec(noinline) int HookedTestFunction()
{
    printf("hook called\n");
    TestFunction_t original = (TestFunction_t)testHook.Trampoline;
    return original(0, 2);
}

int main(int argc, char *argv[])
// int main()
{
	//   lua_State *L = luaL_newstate();
	// 	luaL_openlibs(L);
	// 	lbind_register(L, "double", foobar);
	// 	lbind_dofile(L, "test.lua");
	// 	struct vars * args = lbind_args(L);
	// 	lbind_pushstring(args, "foobar");
	// 	struct vars * result = lbind_call(L, "hello", args);
	// 	assert(lbind_type(result, 0) == LT_INTEGER);
	// 	printf("sizeof 'foobar' = %d\n", lbind_tointeger(result, 0));
	// 	lua_close(L);
	// lua_State *L = luaL_newstate();
	// if(L == NULL)
	// 	return -1;	
	// luaL_openlibs(L);
	// lua_register(L, "foobar", foobar);
	// int ret=luaL_dofile(L, "test.lua");
	// long value = rand()%20;
	// if(ret != 0)
	// {
	// 	printf(" %s\n", lua_tostring(L, -1));
	// }
	// printf(" %d\n", value);
	// luaL_loadstring(L, "print 'hello world'");
	// lua_getglobal(L, "fact");
	// lua_pushinteger(L, value);
	// lua_pcall(L, 1, 1, 0);
	// lua_Number result = lua_tonumber(L, -1);
	// printf("Result from Lua Fact %ld", (long) result);
	// // lua_call(L, 0, 0);
	// lua_close(L);

	res_t rest = {0, BP_INIT_VAL, BP_INIT_VAL};
	int nrand,round;
	/* initialize random */
	srand(122);
	/* randomly produce and consume */
	round = (rand() % 8) + 8;
	while (1)
	{
		nrand = rand() % 16;
		while (nrand--)
		{
			if (consumer(&rest) == FALSE)
				break;
		}
		nrand = rand() % 16;
		while (nrand--)
		{
		if (producer(&rest) == FALSE)
			break;
		}
		if(round < 0)
		break;
		round--;

	}

	/* init gui state */
	// struct nk_context ctx;
	// nk_init_fixed(&ctx, calloc(1, MAX_MEMORY), MAX_MEMORY, &font);
	// enum {EASY, HARD};
	// static int op = EASY;
	// static float value = 0.6f;
	// static int i =  20;
	// if (nk_begin(&ctx, "Show", nk_rect(50, 50, 220, 220),
	//     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
	//     /* fixed widget pixel width */
	//     nk_layout_row_static(&ctx, 30, 80, 1);
	//     if (nk_button_label(&ctx, "button")) {
	//         /* event handling */
	//     }
	//     /* fixed widget window ratio width */
	//     nk_layout_row_dynamic(&ctx, 30, 2);
	//     if (nk_option_label(&ctx, "easy", op == EASY)) op = EASY;
	//     if (nk_option_label(&ctx, "hard", op == HARD)) op = HARD;
	//     /* custom widget pixel width */
	//     nk_layout_row_begin(&ctx, NK_STATIC, 30, 2);
	//     {
	//         nk_layout_row_push(&ctx, 50);
	//         nk_label(&ctx, "Volume:", NK_TEXT_LEFT);
	//         nk_layout_row_push(&ctx, 110);
	//         nk_slider_float(&ctx, 0, &value, 1.0f, 0.1f);
	//     }
	//     nk_layout_row_end(&ctx);
	// }
	// nk_end(&ctx);

	// GdiFont* font;
	// struct nk_context *ctx;
	// WNDCLASSW wc;
	// ATOM atom;
	// RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	// DWORD style = WS_OVERLAPPEDWINDOW;
	// DWORD exstyle = WS_EX_APPWINDOW;
	// HWND wnd;
	// HDC dc;
	// int running = 1;
	// int needs_refresh = 1;
	// /* Win32 */
	// memset(&wc, 0, sizeof(wc));
	// wc.style = CS_DBLCLKS;
	// wc.lpfnWndProc = WindowProc;
	// wc.hInstance = GetModuleHandleW(0);
	// wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	// wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// wc.lpszClassName = L"NuklearWindowClass";
	// atom = RegisterClassW(&wc);
	// AdjustWindowRectEx(&rect, style, FALSE, exstyle);
	// wnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Nuklear GDI Demo",
	//     style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
	//     rect.right - rect.left, rect.bottom - rect.top,
	//     NULL, NULL, wc.hInstance, NULL);
	// dc = GetDC(wnd);
	// /* GUI */
	// font = nk_gdifont_create("Arial", 14);
	// ctx = nk_gdi_init(font, dc, WINDOW_WIDTH, WINDOW_HEIGHT);
	// while (running)
	// {
	//     /* Input */
	//     MSG msg;
	//     nk_input_begin(ctx);
	//     if (needs_refresh == 0) {
	//         if (GetMessageW(&msg, NULL, 0, 0) <= 0)
	//             running = 0;
	//         else {
	//             TranslateMessage(&msg);
	//             DispatchMessageW(&msg);
	//         }
	//         needs_refresh = 1;
	//     } else needs_refresh = 0;
	//     while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
	//         if (msg.message == WM_QUIT)
	//             running = 0;
	//         TranslateMessage(&msg);
	//         DispatchMessageW(&msg);
	//         needs_refresh = 1;
	//     }
	//     nk_input_end(ctx);
	//     /* GUI */
	//     if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 200),
	//         NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
	//         NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
	//     {
	//         enum {EASY, HARD};
	//         static int op = EASY;
	//         static int property = 20;

	//         nk_layout_row_static(ctx, 30, 80, 1);
	//         if (nk_button_label(ctx, "button"))
	//             fprintf(stdout, "button pressed\n");
	//         nk_layout_row_dynamic(ctx, 30, 2);
	//         if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
	//         if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
	//         nk_layout_row_dynamic(ctx, 22, 1);
	//         nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
	//     }
	//     nk_end(ctx);
	//     /* -------------- EXAMPLES ---------------- */
	//     #ifdef INCLUDE_CALCULATOR
	//       calculator(ctx);
	//     #endif
	//     #ifdef INCLUDE_CANVAS
	//       canvas(ctx);
	//     #endif
	//     #ifdef INCLUDE_OVERVIEW
	//       overview(ctx);
	//     #endif
	//     #ifdef INCLUDE_NODE_EDITOR
	//       node_editor(ctx);
	//     #endif
	//     /* ----------------------------------------- */
	//     /* Draw */
	//     nk_gdi_render(nk_rgb(30,30,30));
	// }
	// nk_gdifont_del(font);
	// ReleaseDC(wnd, dc);
	// UnregisterClassW(wc.lpszClassName, wc.hInstance);

	// int menuID, subMenuA, subMenuB;
	// glutInitDisplayString( "stencil~2 rgb double depth>=16 samples" );
	// glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	// glutInitWindowPosition( 100, 100 );
	// glutInit(&argv,argc);
	// subMenuA = glutCreateMenu( SampleMenu );
	// glutAddMenuEntry( "Sub menu A1 (01)", 1 );
	// glutAddMenuEntry( "Sub menu A2 (02)", 2 );
	// glutAddMenuEntry( "Sub menu A3 (03)", 3 );
	// subMenuB = glutCreateMenu( SampleMenu );
	// glutAddMenuEntry( "Sub menu B1 (04)", 4 );
	// glutAddMenuEntry( "Sub menu B2 (05)", 5 );
	// glutAddMenuEntry( "Sub menu B3 (06)", 6 );
	// glutAddSubMenu( "Going to sub menu A", subMenuA );
	// menuID = glutCreateMenu( SampleMenu );
	// glutAddMenuEntry( "Entry one",   1 );
	// glutAddMenuEntry( "Entry two",   2 );
	// glutAddMenuEntry( "Entry three", 3 );
	// glutAddMenuEntry( "Entry four",  4 );
	// glutAddMenuEntry( "Entry five",  5 );
	// glutAddSubMenu( "Enter sub menu A", subMenuA );
	// glutAddSubMenu( "Enter sub menu B", subMenuB );
	// glutCreateWindow( "Hello world!" );
	// glutDisplayFunc( SampleDisplay );
	// glutReshapeFunc( SampleReshape );
	// glutKeyboardFunc( SampleKeyboard );
	// glutSpecialFunc( SampleSpecial );
	// glutIdleFunc( SampleIdle );
	// glutAttachMenu( GLUT_LEFT_BUTTON );
	// glutInitWindowPosition( 200, 200 );
	// glutCreateWindow( "I am not Jan B." );
	// glutDisplayFunc( SampleDisplay );
	// glutReshapeFunc( SampleReshape );
	// glutKeyboardFunc( SampleKeyboard );
	// glutSpecialFunc( SampleSpecial );
	// glutIdleFunc( SampleIdle );
	// glutAttachMenu( GLUT_LEFT_BUTTON );
	// printf( "Testing game mode string parsing, don't panic!\n" );
	// glutGameModeString( "320x240:32@100" );
	// glutGameModeString( "640x480:16@72" );
	// glutGameModeString( "1024x768" );
	// glutGameModeString( ":32@120" );
	// glutGameModeString( "Toudi glupcze, Danwin bedzie moj!" );
	// glutGameModeString( "640x480:16@72" );
	// glutEnterGameMode();
	// glutDisplayFunc( SampleDisplay );
	// glutReshapeFunc( SampleReshape );
	// glutKeyboardFunc( SampleGameModeKeyboard );
	// glutIdleFunc( SampleIdle );
	// glutAttachMenu( GLUT_LEFT_BUTTON );
	// printf( "current window is %ix%i+%i+%i",glutGet( GLUT_WINDOW_X ), glutGet( GLUT_WINDOW_Y ),glutGet( GLUT_WINDOW_WIDTH ), glutGet( GLUT_WINDOW_HEIGHT ));
	// /*
	//   * Enter the main FreeGLUT processing loop
	//   */
	// glutMainLoop();
	// printf( "glutMainLoop() termination works fine!\n" );
	// /*
	//   * This is never reached in FreeGLUT. Is that good?
	//   */
	// return EXIT_SUCCESS;


	int output = TestFunction(0, 1);
	printf("before hook: %u\n", output);
	testHook = CreateHook((void*)&TestFunction, (void*)&HookedTestFunction);
	printf("size: %u\n", testHook.BytesToCopy);
	int status = EnableHook(&testHook);
	printf("status: %u\n", status);
	printf("trampoline: 0x%p\n", testHook.Trampoline);
	output = TestFunction(0, 1);
	printf("after hook: %u\n", output);
	status = DisableHook(&testHook);
	printf("status: %u\n", status);
	output = TestFunction(0, 1);
	printf("disabled hook: %u\n", output);
	status = EnableHook(&testHook);
	printf("status: %u\n", status);
	printf("trampoline: 0x%p\n", testHook.Trampoline);
	output = TestFunction(0, 1);
	printf("after hook: %u\n", output);

	#ifdef WIN32
		system("pause");
	#endif	
	// getchar();
		return 0;
		// return EXIT_SUCCESS;
}
