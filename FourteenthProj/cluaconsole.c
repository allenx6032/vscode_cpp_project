#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>
#include <malloc.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luajit.h"

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

int main( void )
{
	lua_State* L = luaL_newstate();
	luaL_openlibs( L );
	luaL_register( L, LIB_NAME, bindFuncs);
	open_file( L, "scripts/init.lua" );

	lua_close( L );

	return 0;
}