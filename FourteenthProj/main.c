#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

// #include "nio_ce.h"

#undef NDEBUG
#include <assert.h>
#include <debug.h>

#include "minilua.h"


static char text[401] = {0};




static void stackDump(lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	printf("----lua stack dump----  ");
	for (i = 1; i<= top; i++) {
		int t = lua_type(L, i);
		switch(t) {
			case LUA_TSTRING:{
				printf("'%s'", lua_tostring(L, i));
				break;
			}
			case LUA_TBOOLEAN: {
				printf(lua_toboolean(L, i)?"true":"false");
				break;
			}
			case LUA_TNUMBER: {
				printf("%g", lua_tonumber(L, i));
				break;
			}
			default: {
				printf("%s", lua_typename(L, t));
				break;
			}
		}
		
		printf(" ");
	}
	printf("\n");
}

#define MAX_COLOR 255

int getfield (lua_State * L, const char *key)
{
	int result;
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if (!lua_isnumber(L, -1)) printf("lua_isnumber invalid\n");
	result = (int)(lua_tonumber(L, -1)*MAX_COLOR);
	stackDump(L);
	lua_pop(L, 1); // 删除数字
	stackDump(L);
	
	printf("result: %d\n", result);
	
	return result;
}

int luaAdd(lua_State * L , int x, int y) {
	int sum = -1;
	lua_getglobal(L, "addInt");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	stackDump(L);
	lua_call(L, 2, 1); // lua_call方法调用完毕之后，会自动清空调用之前所放入的函数名称以及参数，并且把调用结果放入到
	stackDump(L);
	sum = lua_tointeger(L, -1); // 注意，这类函数仅仅返回值，但是并不对栈进行操作
	stackDump(L);
	lua_pop(L, 1); // 从栈中弹出一个元素，注意这些函数仅仅只操作栈，但是并不对返回值
	return sum;
}


void main(void)
{
    // nio_console c1;
    // bool declared = false;
    // lua_State* L = luaL_newstate();
    // nio_init(&c1, NIO_MAX_COLS, NIO_MAX_ROWS, 0, 0, NIO_COLOR_BLACK, NIO_COLOR_WHITE, true);
    // nio_set_default(&c1);
    // nio_printf("MINILUA built on %s, %s\n\n", __DATE__, __TIME__);
    // luaL_openlibs(L); // Load Lua libraries
    // while(1)
    // {
    //     nio_printf("> ");
    //     // If no text was entered, exit
    //     if(!nio_getsn(text, 400))
    //         continue;
    //     if (!strcmp(text, "clear")) {
    //         nio_clear(&c1);
    //     }
    //     else if (!strcmp(text, "exit")) {
    //         break;
    //     }
    //     else if (!strcmp(text, "undef")) {
    //         if (declared) {
    //             lua_pushnil(L);
    //             lua_setglobal(L, "func");
    //             declared = false;
    //         } else {
    //             nio_printf("Function 'func(n) ... end' was not declared\n\n");
    //         }
    //     }
    //     else if (strstr(text, "def ") == text) { // for now, let the user declare a func(n) function, returning an int
    //         //luaL_loadbuffer(L, c_function, strlen(c_function), c_function);
    //         luaL_loadbuffer(L, text+4, strlen(text+4), text+4);
    //         if (lua_pcall(L,0,0,0) == 0) {
    //             declared = true;
    //             nio_printf("Function 'func(n) ... end' declared OK\n\n");
    //         } else {
    //             declared = false;
    //             nio_printf("Function could not get declared\n\n");
    //         }
    //     }
    //     else if (strstr(text, "exec ") == text)
    //     {
    //         if (!declared) {
    //             nio_printf("Function 'func(n) ... end' is not declared\n\n");
    //         } else {
    //             int n = atoi(text+5);
    //             int r = -1;
    //             dbg_sprintf((char*)dbgout, "calling func(%d)...\n", n);
    //             lua_getfield(L, -10002 /* LUA_GLOBALSINDEX */, "func");
    //             lua_pushinteger(L, n);
    //             lua_call(L,1,1); // call func, with one argument, and expect one result
    //             dbg_sprintf((char*)dbgout, "     after lua_call\n");
    //             r = luaL_checkinteger(L, -1); // the top of the Lua stack should contain our result
    //             nio_printf("Function result: %d\n\n", r);
    //         }
    //     } else {
    //         nio_printf("Unknown command\n\n");
    //     }
    // }
    // lua_close(L);
    // nio_free(&c1);


    lua_State *L = luaL_newstate();
	luaL_openlibs(L); // 加载这个方法避免lua中无法调用系统库的错误，http://lua-users.org/lists/lua-l/2014-05/msg00279.html

	char filePath[100];
	getcwd(filePath, 100);
	strcat(filePath, "/call_lua.lua");
	printf("file path: %s\n", filePath);
	// lua文件只用load一次即可，并不会消失
	if (luaL_loadfile(L, filePath)) { 
		printf("luaL_loadfile \n");
	}
	if ( lua_pcall(L, 0, 0, 0)) {
		printf("lua_pcall error\n");
	}

	// 获取lua中的全局变量
	lua_getglobal(L, "background");
	if (!lua_istable(L, -1)) {
		printf("lua_istable error");
	}
	int red = getfield(L, "r");
	int green = getfield(L, "g");
	int blue = getfield(L, "b");
	printf("r: %d, g: %d b: %d \n",red, green,blue);
	// 调用lua中的方法
	int sum = luaAdd(L, 1, 5);
	printf("C luaAdd function result: %d\n", sum);
	lua_close(L);
	L = NULL;
    return;
}
