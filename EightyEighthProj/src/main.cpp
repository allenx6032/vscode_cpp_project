
#include "stdio.h"
#include "stdlib.h"
#include "ELuna.h"
#include <luaMagic.hpp>
#include <cstring>
#include <iostream>
//Create video using:
//./netPbmRender | ffmpeg -i pipe:0 -c:v libx264rgb VideoName.avi
double add(double x, double y);
void upper(const char* str);
void hello(const char* str, int x, double y);

int main()
{

	// /* 创建lua_State */
    // lua_State* ls = luaL_newstate();
    // /* 创建失败，退出 */
    // if(ls == NULL)
    // {
    //     fprintf(stderr," Cannot create state.\n");
    //     return 1;
    // }
    // /* 打开所有Lua标准库 */
    // luaL_openlibs(ls);
	// /* 绑定函数 */
	// luaMagic::bind(ls, "add", add);
	// luaMagic::bind(ls, "upper", upper);
	// luaMagic::bind(ls, "hello", hello);

	lua_State *L = ELuna::openLua();
	ELuna::registerFunction(L, "add", add);
	ELuna::registerFunction(L, "upper", upper);
    ELuna::registerFunction(L, "hello", hello);
	ELuna::doFile(L, "hello.lua");
	ELuna::closeLua(L);
	return 0;
}

double add(double x, double y)
{
	return x+y;
}

void upper(const char* str)
{
	for(int i = 0 ; i < strlen(str); i++)
	{
		putchar(str[i] & ~(1<<5));
	}
	putchar('\n');
}

void hello(const char* str, int x, double y)
{
	printf("%s %d %lf\n", str, x, y);
}

