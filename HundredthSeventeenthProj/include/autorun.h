#include <ege.h>
#include <ege_graphics.h>
#include <ege_lua.h>

typedef struct ege4lua_functons
{
	char key[32];
	int (*value)(lua_State* L);
}ege4lua_functons;


typedef struct ege4lua_constant
{
	char key[32];
	int value;
}ege4lua_constant;

/* 常量 */
ege4lua_constant consts[] = 
{
	/* render mode */
	{"RENDER_AUTO",				RENDER_AUTO},
	{"RENDER_MANUAL",			RENDER_MANUAL},
	/* line style */
	{"SOLID_LINE",				SOLID_LINE},
	{"CENTER_LINE",				CENTER_LINE},
	{"DOTTED_LINE",				DOTTED_LINE},
	{"DASHED_LINE",				DASHED_LINE},
	{"NULL_LINE",				NULL_LINE},



	/* key code */
	{"KEY_COMMA",				188},
	{"KEY_SUB",					189},
	{"KEY_DOT",					190},
	{"KEY_LEFTSKEWLINE",		191},
	{"KEY_SEPARATION",			192},
	{"KEY_LEFTSQUARBRACKET",	219},
	{"KEY_RIGHTSKEWLINE",		220},
	{"KEY_RIGHTSQUARBRACKET",	221},
	{"KEY_QUOTATION",			222},
	{"KEY_0",					'0'},
	{"KEY_1",					'1'},
	{"KEY_2",					'2'},
	{"KEY_3",					'3'},
	{"KEY_4",					'4'},
	{"KEY_5",					'5'},
	{"KEY_6",					'6'},
	{"KEY_7",					'7'},
	{"KEY_8",					'8'},
	{"KEY_9",					'9'},
	{"KEY_A",					'A'},
	{"KEY_B",					'B'},
	{"KEY_C",					'C'	},
	{"KEY_D",					'D'},
	{"KEY_E",					'E'},
	{"KEY_F",					'F'},
	{"KEY_G",					'G'},
	{"KEY_H",					'H'},
	{"KEY_I",					'I'},
	{"KEY_J",					'J'},
	{"KEY_K",					'K'},
	{"KEY_L",					'L'},
	{"KEY_M",					'M'},
	{"KEY_N",					'N'},
	{"KEY_O",					'O'},
	{"KEY_P",					'P'},
	{"KEY_Q",					'Q'},
	{"KEY_R",					'R'},
	{"KEY_S",					'S'},
	{"KEY_T",					'T'},
	{"KEY_U",					'U'},
	{"KEY_V",					'V'},
	{"KEY_W",					'W'},
	{"KEY_X",					'X'},
	{"KEY_Y",					'Y'},
	{"KEY_Z",					'Z'},	
	{"KEY_ESC",					VK_ESCAPE},
	{"KEY_ENTER",				VK_RETURN},
	{"KEY_TAB",					VK_TAB},
	{"KEY_CAPSLOCK",			VK_CAPITAL},
	{"KEY_SHIFT",				VK_SHIFT},
	{"KEY_CTRL",				VK_CONTROL},
	{"KEY_ALT",					VK_MENU},
	{"KEY_SPACE",				VK_SPACE},
	{"KEY_BACKSPACE",			VK_BACK},
	{"KEY_WIN",					VK_LWIN},
	{"KEY_APPS",				VK_APPS},
	{"KEY_INSTERT",				VK_INSERT},
	{"KEY_HOME",				VK_HOME},
	{"KEY_PAGEUP",				VK_PRIOR},
	{"KEY_PAGEDOWN",			VK_NEXT},
	{"KEY_END",					VK_END},
	{"KEY_DELETE",				VK_DELETE},
	{"KEY_LEFT",				VK_LEFT},
	{"KEY_UP",					VK_UP},
	{"KEY_RIGHT",				VK_RIGHT},
	{"KEY_DOWN",				VK_DOWN},
	{"KEY_F1",					VK_F1},
	{"KEY_F2",					VK_F2},
	{"KEY_F3",					VK_F3},
	{"KEY_F4",					VK_F4},
	{"KEY_F5",					VK_F5},
	{"KEY_F6",					VK_F6},
	{"KEY_F7",					VK_F7},
	{"KEY_F8",					VK_F8},
	{"KEY_F9",					VK_F9},
	{"KEY_F10",					VK_F10},
	{"KEY_F11",					VK_F11},
	{"KEY_F12",					VK_F12},
	{"KEY_NUMLOCK",				VK_NUMLOCK},
	{"KEY_NUM0",				VK_NUMPAD0},
	{"KEY_NUM1",				VK_NUMPAD1},
	{"KEY_NUM2",				VK_NUMPAD2},
	{"KEY_NUM3",				VK_NUMPAD3},
	{"KEY_NUM4",				VK_NUMPAD4},
	{"KEY_NUM5",				VK_NUMPAD5},
	{"KEY_NUM6",				VK_NUMPAD6},
	{"KEY_NUM7",				VK_NUMPAD7},
	{"KEY_NUM8",				VK_NUMPAD8},
	{"KEY_NUM9",				VK_NUMPAD9},
	{"KEY_NUMDOT",				VK_DECIMAL},
	{"KEY_NUMADD",				VK_ADD},
	{"KEY_NUMSUB",				VK_SUBTRACT},
	{"KEY_NUMMUL",				VK_MULTIPLY},
	{"KEY_NUMDIV",				VK_DIVIDE},

};

/* 函数 */
ege4lua_functons funcs[] = 
{
	/* 绘图环境相关函数 */
	{"cleardevice",			ege4lua_cleardevice},
	{"clearviewport",		ege4lua_clearviewport},
	{"closegraph",			ege4lua_closegraph},
	{"gettarget",			ege4lua_gettarget},
	{"getviewport",			ege4lua_getviewport},
	{"initgraph",			ege4lua_initgraph},
	{"isrun",				ege4lua_isrun},
	{"setactivepage",		ege4lua_setactivepage},
	{"setcaption",			ege4lua_setcaption},
	{"setinitmode",			ege4lua_setinitmode},
	{"setrendermode",		ege4lua_setrendermode},
	{"settarget",			ege4lua_settarget},
	{"setviewport",			ege4lua_setviewport},
	{"setvisualpage",		ege4lua_setvisualpage},
	{"window_getviewport",	ege4lua_window_getviewport},
	{"window_setviewport",	ege4lua_window_setviewport},

	/* 绘图参数(颜色、风格、当前点等)相关函数 */
	{"getred",				ege4lua_getred},
	{"getgreen",			ege4lua_getgreen},
	{"getblue",				ege4lua_getblue},
	{"getbkcolor",			ege4lua_getbkcolor},
	{"getcolor",			ege4lua_getcolor},
	{"getfillcolor",		ege4lua_getfillcolor},
	{"hsl2rgb",				ege4lua_hsl2rgb},
	{"hsv2rgb",				ege4lua_hsv2rgb},
	{"rgb2gray",			ege4lua_rgb2gray},
	{"rgb2hsl",				ege4lua_rgb2hsl},
	{"rgb2hsv",				ege4lua_rgb2hsv},
	{"setbkcolor",			ege4lua_setbkcolor},
	{"setclrcolor",			ege4lua_setclrcolor},
	{"setbkmode",			ege4lua_setbkmode},
	{"setcolor",			ege4lua_setcolor},
	{"setfillcolor",		ege4lua_setfillcolor},
	{"setfontbkcolor",		ege4lua_setfontbkcolor},
	{"setfillstyle",		ege4lua_setfillstyle},
	{"setlinestyle",		ege4lua_setlinestyle},
	{"setlinewidth",		ege4lua_setlinewidth},
	{"getheight",			ege4lua_getheight},
	{"getwidth",			ege4lua_getwidth},
	{"getsize",				ege4lua_getsize},
	{"getx",				ege4lua_getx},
	{"gety",				ege4lua_gety},
	{"getposition",			ege4lua_getposition},
	{"getpixelcolor",		ege4lua_getpixelcolor},
	{"moverel",				ege4lua_moverel},
	{"moveto",				ege4lua_moveto},


	/* 绘制图形相关函数 */
	{"arc",					ege4lua_arc},
	{"bar",					ege4lua_bar},
	{"bar3d",				ege4lua_bar3d},
	{"circle",				ege4lua_circle},
	{"drawbezier",			ege4lua_drawbezier},
	{"drawlines",			ege4lua_drawlines},
	{"drawpoly",			ege4lua_drawpoly},
	{"ellipse",				ege4lua_ellipse},
	{"fillellipse",			ege4lua_fillellipse},
	{"fillpoly",			ege4lua_fillpoly},
	{"floodfill",			ege4lua_floodfill},
	{"floodfillsurface",	ege4lua_floodfillsurface},
	{"line",				ege4lua_line},
	{"linerel",				ege4lua_linerel},
	{"lineto",				ege4lua_lineto},
	{"sector",				ege4lua_sector},
	{"ellipticsector",		ege4lua_ellipticsector},
	{"rectangle",			ege4lua_rectangle},
	{"putpixel",			ege4lua_putpixel},

	/* 文字输出相关函数 */
	{"setfont",				ege4lua_setfont},
	{"xyprint",				ege4lua_xyprint},

	/* 图像处理相关函数 */
	{"putimage",			ege4lua_putimage},
	{"getimage",			ege4lua_getimage},
	{"screenshot",			ege4lua_screenshot},

	/* 键盘鼠标输入函数 */
	{"getch",				ege4lua_getch},
	{"keystate",			ege4lua_keystate},

	/* 时间函数 */
	{"delay_fps",			ege4lua_delay_fps},

	/* 其它函数 */
	{"getfps",				ege4lua_getfps},
};

static inline void toregister(lua_State* L)
{
	lua_newtable(L);			//创建一张表(1)  -3

	/* 注册函数到Lua虚拟机 */
	unsigned int i = 0;
	for(i=0;i<sizeof(funcs)/sizeof(ege4lua_functons);i++)
	{
		lua_pushstring(L,funcs[i].key);//key  -2
		lua_pushcfunction(L,funcs[i].value);//value  -1
		lua_settable(L,-3);
	}
	/* 注册常量到Lua虚拟机 */
	for(i=0;i<sizeof(consts)/sizeof(ege4lua_constant);i++)
	{
		lua_pushstring(L,consts[i].key);//key  -2
		lua_pushinteger(L,consts[i].value);//value  -1
		lua_settable(L,-3);
	}

	lua_setglobal(L,"ege");		//将(1)处创建的表命名为ege
}