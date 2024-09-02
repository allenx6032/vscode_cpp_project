/*

    This file is part of gl3w, hosted at https://github.com/skaslev/gl3w

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
// #include <GL/gl3w.h>
#include <GL/glfw.h>
#include <math.h>
#include <io.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winerror.h>
#include <Windows.h>
#include <malloc.h>

#include "lautoc.h"
#ifndef LUA_IMPL
#define LUA_IMPL
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif



#ifndef M_PI
#define M_PI 3.141592654
#endif

static int running = 1;
static int autoexit = 0;


static int width = 600, height = 600;

static float randf()
{
	return (float) rand() / ((float) RAND_MAX + 1);
}

static void
gear(GLfloat inner_radius, GLfloat outer_radius, GLfloat width,
  GLint teeth, GLfloat tooth_depth)
{
  GLint i;
  GLfloat r0, r1, r2;
  GLfloat angle, da;
  GLfloat u, v, len;
  r0 = inner_radius;
  r1 = outer_radius - tooth_depth / 2.f;
  r2 = outer_radius + tooth_depth / 2.f;
  da = 2.f * (float) M_PI / teeth / 4.f;
  glShadeModel(GL_FLAT);
  glNormal3f(0.f, 0.f, 1.f);
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), width * 0.5f);
    if (i < teeth) {
      glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), width * 0.5f);
      glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), width * 0.5f);
    }
  }
  glEnd();
  glBegin(GL_QUADS);
  da = 2.f * (float) M_PI / teeth / 4.f;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), width * 0.5f);
  }
  glEnd();
  glNormal3f(0.0, 0.0, -1.0);
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), -width * 0.5f);
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), -width * 0.5f);
    if (i < teeth) {
      glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), -width * 0.5f);
      glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), -width * 0.5f);
    }
  }
  glEnd();
  glBegin(GL_QUADS);
  da = 2.f * (float) M_PI / teeth / 4.f;
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), -width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), -width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), -width * 0.5f);
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), -width * 0.5f);
  }
  glEnd();
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i < teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle), r1 * (float) sin(angle), -width * 0.5f);
    u = r2 * (float) cos(angle + da) - r1 * (float) cos(angle);
    v = r2 * (float) sin(angle + da) - r1 * (float) sin(angle);
    len = (float) sqrt(u * u + v * v);
    u /= len;
    v /= len;
    glNormal3f(v, -u, 0.0);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + da), r2 * (float) sin(angle + da), -width * 0.5f);
    glNormal3f((float) cos(angle), (float) sin(angle), 0.f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), width * 0.5f);
    glVertex3f(r2 * (float) cos(angle + 2 * da), r2 * (float) sin(angle + 2 * da), -width * 0.5f);
    u = r1 * (float) cos(angle + 3 * da) - r2 * (float) cos(angle + 2 * da);
    v = r1 * (float) sin(angle + 3 * da) - r2 * (float) sin(angle + 2 * da);
    glNormal3f(v, -u, 0.f);
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), width * 0.5f);
    glVertex3f(r1 * (float) cos(angle + 3 * da), r1 * (float) sin(angle + 3 * da), -width * 0.5f);
    glNormal3f((float) cos(angle), (float) sin(angle), 0.f);
  }
  glVertex3f(r1 * (float) cos(0), r1 * (float) sin(0), width * 0.5f);
  glVertex3f(r1 * (float) cos(0), r1 * (float) sin(0), -width * 0.5f);
  glEnd();
  glShadeModel(GL_SMOOTH);
  glBegin(GL_QUAD_STRIP);
  for (i = 0; i <= teeth; i++) {
    angle = i * 2.f * (float) M_PI / teeth;
    glNormal3f(-(float) cos(angle), -(float) sin(angle), 0.f);
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), -width * 0.5f);
    glVertex3f(r0 * (float) cos(angle), r0 * (float) sin(angle), width * 0.5f);
  }
  glEnd();
}

static GLfloat view_rotx = 20.f, view_roty = 30.f, view_rotz = 0.f;
static GLint gear1, gear2, gear3;
static GLfloat angle = 0.f;

static void draw(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  glRotatef(view_rotx, 1.0, 0.0, 0.0);
  glRotatef(view_roty, 0.0, 1.0, 0.0);
  glRotatef(view_rotz, 0.0, 0.0, 1.0);
  glPushMatrix();
  glTranslatef(-3.0, -2.0, 0.0);
  glRotatef(angle, 0.0, 0.0, 1.0);
  glCallList(gear1);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(3.1f, -2.f, 0.f);
  glRotatef(-2.f * angle - 9.f, 0.f, 0.f, 1.f);
  glCallList(gear2);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(-3.1f, 4.2f, 0.f);
  glRotatef(-2.f * angle - 25.f, 0.f, 0.f, 1.f);
  glCallList(gear3);
  glPopMatrix();
  glPopMatrix();
}

static void animate(void)
{
  angle = 100.f * (float) glfwGetTime();
}

void key( int k, int action )
{
  if( action != GLFW_PRESS ) return;
  switch (k) {
  case 'Z':
    if( glfwGetKey( GLFW_KEY_LSHIFT ) )
      view_rotz -= 5.0;
    else
      view_rotz += 5.0;
    break;
  case GLFW_KEY_ESC:
    running = 0;
    break;
  case GLFW_KEY_UP:
    view_rotx += 5.0;
    break;
  case GLFW_KEY_DOWN:
    view_rotx -= 5.0;
    break;
  case GLFW_KEY_LEFT:
    view_roty += 5.0;
    break;
  case GLFW_KEY_RIGHT:
    view_roty -= 5.0;
    break;
  default:
    return;
  }
}

void reshape( int width, int height )
{
  GLfloat h = (GLfloat) height / (GLfloat) width;
  GLfloat xmax, znear, zfar;
  znear = 5.0f;
  zfar  = 30.0f;
  xmax  = znear * 0.5f;
  glViewport( 0, 0, (GLint) width, (GLint) height );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -xmax, xmax, -xmax*h, xmax*h, znear, zfar );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glTranslatef( 0.0, 0.0, -20.0 );
}

static void init(int argc, char *argv[])
{
  static GLfloat pos[4] = {5.f, 5.f, 10.f, 0.f};
  static GLfloat red[4] = {0.8f, 0.1f, 0.f, 1.f};
  static GLfloat green[4] = {0.f, 0.8f, 0.2f, 1.f};
  static GLfloat blue[4] = {0.2f, 0.2f, 1.f, 1.f};
  GLint i;
  glLightfv(GL_LIGHT0, GL_POSITION, pos);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  gear1 = glGenLists(1);
  glNewList(gear1, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
  gear(1.f, 4.f, 1.f, 20, 0.7f);
  glEndList();
  gear2 = glGenLists(1);
  glNewList(gear2, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
  gear(0.5f, 2.f, 2.f, 10, 0.7f);
  glEndList();
  gear3 = glGenLists(1);
  glNewList(gear3, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
  gear(1.3f, 2.f, 0.5f, 10, 0.7f);
  glEndList();
  glEnable(GL_NORMALIZE);
  for ( i=1; i<argc; i++ ) {
    if (strcmp(argv[i], "-info")==0) {
      printf("GL_RENDERER   = %s\n", (char *) glGetString(GL_RENDERER));
      printf("GL_VERSION    = %s\n", (char *) glGetString(GL_VERSION));
      printf("GL_VENDOR     = %s\n", (char *) glGetString(GL_VENDOR));
      printf("GL_EXTENSIONS = %s\n", (char *) glGetString(GL_EXTENSIONS));
    }
    else if ( strcmp(argv[i], "-exit")==0) {
      autoexit = 30;
      printf("Auto Exit after %i seconds.\n", autoexit );
    }
  }
}

void dump(lua_State * L) {
	int n = lua_gettop(L);
	int i;
	for (i = 1; i <= n; i++) {
		int type = lua_type(L, i);
		switch (type) {
		case LUA_TNUMBER:
			printf("%d : %g\n", i, lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			printf("%d : %s\n", i, lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			printf("%d : %s\n", i, lua_toboolean(L, i) ? "true" : "false");
			break;
		case LUA_TFUNCTION:
			printf("%d : function\n", i);
			break;
		default:
			printf("%d : %s\n", i, type);
			break;
		}
	}
}

int add(lua_State *L) {
	double n1 = lua_tonumber(L, -1);
	double n2 = lua_tonumber(L, -2);
	lua_pushnumber(L, n1 + n2);
	return 1; // 返回值代表该函数的返回值个数
}

void c_call_lua() {

	lua_State * L = luaL_newstate(); // 创建 Lua 与 C 进行数据交互的堆栈并返回指针 L；
	luaL_openlibs(L);                //为堆栈加载所有的标准 Lua 库，支持脚本在 C 应用中执行
	//luaL_dostring(L, "print('from dostring')");  // 执行 lua 脚本
	//luaL_dofile(L,"D:\\eclipse-workspace\\c_lua\\src\\a.lua");          //执行整个 Lua 脚本文件 a.lua
	luaL_loadfile(L, "a.lua");
	lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "power");
	lua_pushinteger(L, 2);
	lua_pushinteger(L, 10);
	//dump(L);
	lua_call(L, 2, 1);
	//dumpStack(L);
	int res = lua_tonumber(L, -1);  // 取出栈顶结果
	printf("power:%d", res);
	lua_close(L);                   //函数关闭堆栈，释放资源
	L = NULL;
}

void lua_call_c() {
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "add", add);
	luaL_dofile(L, "a.lua");
	lua_close(L);
	L = NULL;
}

static void stack_dump(lua_State *L)
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
	stack_dump(L);
	lua_pop(L, 1); // 删除数字
	stack_dump(L);
	printf("result: %d\n", result);
	return result;
}

void getfieldlua (lua_State * L, const char *key)
{
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	stack_dump(L);
	lua_pop(L, 1);
}

int average(lua_State *L) {
	int n = lua_gettop(L);
	double sum = 0;
	int i = 1;
	while(i <= n) {
		sum += lua_tonumber(L, i);
		i ++;
	}
	// 设置lua中调用时的函数返回值
	lua_pushnumber(L, sum/n);
	lua_pushnumber(L, sum);
	// 这个返回值是告诉lua虚拟机返回值的个数，以便支持多返回值函数调用
	return 2;
}

int luaAdd(lua_State * L , int x, int y) {
	int sum = -1;
	lua_getglobal(L, "addInt");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	stack_dump(L);
	lua_call(L, 2, 1); // lua_call方法调用完毕之后，会自动清空调用之前所放入的函数名称以及参数，并且把调用结果放入到
	stack_dump(L);
	sum = lua_tointeger(L, -1); // 注意，这类函数仅仅返回值，但是并不对栈进行操作
	stack_dump(L);
	lua_pop(L, 1); // 从栈中弹出一个元素，注意这些函数仅仅只操作栈，但是并不对返回值
	return sum;
}


void lua_stack_dump()
{
	lua_State *L = luaL_newstate();
	// lua stack栈顶索引为最大值，通过lua_gettop()获取这个值，栈底索引为1；同时可以使用负值来进行操作，比如-1表示栈顶，而-n表示栈底部；这个索引的用法切记！！！
	lua_pushboolean(L, 1);
	lua_pushnumber(L, 10);
	lua_pushnil(L);
	// 向stack中push字符串的时候，stack会对字符串进行复制!!!
	char * str = (char *)malloc(sizeof(char) * 20);
	strcpy(str, "hello");
	lua_pushstring(L, str);
	free(str);
	stack_dump(L); // 从栈底部开始打印： true 10 nil hello
	lua_pushvalue(L, -3); // 将指定索引上的副本压入栈顶部，即再压入一个1   true 10 nil hello 10
	stack_dump(L);
	lua_replace(L, 3); // 弹出栈顶的值，并将该值放到指定的索引上，即将栈顶的1放到nil的位置上 ture 10 true hello
	stack_dump(L);
	lua_settop(L, 6); // 设置栈顶位置，即添加两个nil, true 10 true hello nil nil
	stack_dump(L);
	lua_remove(L, -3); // 删除指定索引上的值，并将上面的值下移，即删除hello，  true 10 true nil nil
	stack_dump(L);
	lua_settop(L, -5); // true
	stack_dump(L);
	lua_close(L);
	L = NULL;
}


void c_call_lua_add() {
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
}

void lua_call_c_average()
{
	lua_State *L = luaL_newstate();
	// 加载这个方法避免lua中无法调用系统库的错误，http://lua-users.org/lists/lua-l/2014-05/msg00279.html
	// 在"lualib.h"中，有各种各样的基础库的类型，如果仅仅只是使用一些基本库，可以只加载比如"base"库即可，参考：http://cn.cocos2d-x.org/tutorial/show?id=1213
	// 这里我们偷懒一下，把所有的lua的基本库都加载进去
	luaL_openlibs(L); 
	lua_register(L, "average", average);
	char filePath[100];
	getcwd(filePath, 100);
	strcat(filePath, "/call_c.lua");
	printf("file path: %s\n", filePath);
	if (luaL_loadfile(L, filePath)) { 
		printf("luaL_loadfile error\n");
	}
	if ( lua_pcall(L, 0, 0, 0)) {
		printf("lua_pcall error\n");
	}
	lua_getglobal(L, "result");
	if (!lua_istable(L, -1)) {
		printf("lua_istable error");
	}
	getfieldlua(L, "avg");
	getfieldlua(L, "sum");
	lua_close(L);
	L = NULL;
}


#define LIB_NAME "clua"

static void open_file(lua_State* L, const char* filename){
	if (luaL_dofile(L, filename) != LUA_OK){
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

float power(float val, int pow) {
  float x = 1.0;
  for(int i = 0; i < pow; i++) {
    x = x * val;
  }
  return x;
}

int main(int argc, char **argv)
{

	c_call_lua();
	printf("\nc_call_lua\n");
	getchar();
	
	lua_call_c();
	printf("\nlua_call_c\n");
	getchar();

	lua_stack_dump();
	printf("\nlua_stack_dump\n");
	getchar();

	c_call_lua_add();
	printf("\nc_call_lua_add\n");
	getchar();

	lua_call_c_average();
	printf("\nlua_call_c_average\n");
	getchar();

	lua_State* L = luaL_newstate();
	luaL_openlibs( L );
	luaL_register( L, LIB_NAME, bindFuncs);
	open_file( L, "init.lua" );
	lua_close( L );

 	L = luaL_newstate();
	luaA_open(L);
	luaA_function(L, power, float, float, int);
	lua_pushnumber(L, 4.2);
	lua_pushinteger(L, 3);
	luaA_call(L, power);
	printf("Result: %f\n", lua_tonumber(L, -1));
	lua_pop(L, 1);
	luaA_close(L);
	lua_close(L);

	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		exit( EXIT_FAILURE );
	}
	if( !glfwOpenWindow( 300,300, 0,0,0,0, 16,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window\n" );
		getchar();
		glfwTerminate();
		exit( EXIT_FAILURE );
	}
	glfwSetWindowTitle( "Gears" );
	glfwEnable( GLFW_KEY_REPEAT );
	glfwSwapInterval( 1 );
	init(argc, argv);
	glfwSetWindowSizeCallback( reshape );
	glfwSetKeyCallback( key );
	while( running )
	{
		draw();
		animate();
		glfwSwapBuffers();
		if( !glfwGetWindowParam( GLFW_OPENED ) )
		{
			running = 0;
		}
	}
	getchar();
	glfwTerminate();
	exit( EXIT_SUCCESS );
	// return 0;
}
