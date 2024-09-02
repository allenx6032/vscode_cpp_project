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
#include <stddef.h>
#include <Windows.h>
#include <malloc.h>


# define _CRT_SECURE_NO_WARNINGS
#include "genann.h"
#include "minctest.h"
#include <math.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h> 
#include <time.h> 
#include <stdint.h>



#include "ini.h"
#include "mpool.h"
#include "c_vector.h"
#include "thpool.h"
#include "cJSON.h"
#include "logger.h"
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "fastlz.h"
#include "lz4.h"

#include "gd.h"
enum { left, center, right };

/* nuklear - 1.32.0 - public domain */
#define WIN32_LEAN_AND_MEAN
#include <string.h>
#include <time.h>
#include <limits.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// #define WINDOW_WIDTH 800
// #define WINDOW_HEIGHT 600
static int WINDOW_WIDTH = 600, WINDOW_HEIGHT = 600;


#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "l_crypt.h"
#include "l_sqlite3.h"
#include "l_lfs.h"
#include "l_xml.h"
#include "ls_luasocket.h"
#include "luagl.h"
#include "luaglu.h"


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

int bsz = 8;
int times = 100000;
int runtimes = 100;

void basic() {
    genann *ann = genann_init(1, 0, 0, 1);
    lequal(ann->total_weights, 2);
    double a;
    a = 0;
    ann->weight[0] = 0;
    ann->weight[1] = 0;
    lfequal(0.5, *genann_run(ann, &a));
    a = 1;
    lfequal(0.5, *genann_run(ann, &a));
    a = 11;
    lfequal(0.5, *genann_run(ann, &a));
    a = 1;
    ann->weight[0] = 1;
    ann->weight[1] = 1;
    lfequal(0.5, *genann_run(ann, &a));
    a = 10;
    ann->weight[0] = 1;
    ann->weight[1] = 1;
    lfequal(1.0, *genann_run(ann, &a));
    a = -10;
    lfequal(0.0, *genann_run(ann, &a));
    genann_free(ann);
}

void xor() {
    genann *ann = genann_init(2, 1, 2, 1);
    ann->activation_hidden = genann_act_threshold;
    ann->activation_output = genann_act_threshold;
    lequal(ann->total_weights, 9);
    /* First hidden. */
    ann->weight[0] = .5;
    ann->weight[1] = 1;
    ann->weight[2] = 1;
    /* Second hidden. */
    ann->weight[3] = 1;
    ann->weight[4] = 1;
    ann->weight[5] = 1;
    /* Output. */
    ann->weight[6] = .5;
    ann->weight[7] = 1;
    ann->weight[8] = -1;
    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 1, 1, 0};
    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));
    genann_free(ann);
}

void backprop() {
    genann *ann = genann_init(1, 0, 0, 1);
    double input, output;
    input = .5;
    output = 1;
    double first_try = *genann_run(ann, &input);
    genann_train(ann, &input, &output, .5);
    double second_try = *genann_run(ann, &input);
    lok(fabs(first_try - output) > fabs(second_try - output));
    genann_free(ann);
}

void train_and() {
    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 0, 0, 1};
    genann *ann = genann_init(2, 0, 0, 1);
    int i, j;
    for (i = 0; i < 50; ++i) {
        for (j = 0; j < 4; ++j) {
            genann_train(ann, input[j], output + j, .8);
        }
    }
    ann->activation_output = genann_act_threshold;
    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));
    genann_free(ann);
}

void train_or() {
    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 1, 1, 1};
    genann *ann = genann_init(2, 0, 0, 1);
    genann_randomize(ann);
    int i, j;
    for (i = 0; i < 50; ++i) {
        for (j = 0; j < 4; ++j) {
            genann_train(ann, input[j], output + j, .8);
        }
    }
    ann->activation_output = genann_act_threshold;
    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));
    genann_free(ann);
}

void train_xor() {
    double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    double output[4] = {0, 1, 1, 0};
    genann *ann = genann_init(2, 1, 2, 1);
    int i, j;
    for (i = 0; i < 500; ++i) {
        for (j = 0; j < 4; ++j) {
            genann_train(ann, input[j], output + j, 3);
        }
        /* printf("%1.2f ", xor_score(ann)); */
    }
    ann->activation_output = genann_act_threshold;
    lfequal(output[0], *genann_run(ann, input[0]));
    lfequal(output[1], *genann_run(ann, input[1]));
    lfequal(output[2], *genann_run(ann, input[2]));
    lfequal(output[3], *genann_run(ann, input[3]));
    genann_free(ann);
}

void persist() {
    genann *first = genann_init(1000, 5, 50, 10);
    FILE *out = fopen("persist.txt", "w");
    genann_write(first, out);
    fclose(out);
    FILE *in = fopen("persist.txt", "r");
    genann *second = genann_read(in);
    fclose(in);
    lequal(first->inputs, second->inputs);
    lequal(first->hidden_layers, second->hidden_layers);
    lequal(first->hidden, second->hidden);
    lequal(first->outputs, second->outputs);
    lequal(first->total_weights, second->total_weights);
    int i;
    for (i = 0; i < first->total_weights; ++i) {
        lok(first->weight[i] == second->weight[i]);
    }
    genann_free(first);
    genann_free(second);
}

void copy() {
    genann *first = genann_init(1000, 5, 50, 10);
    genann *second = genann_copy(first);
    lequal(first->inputs, second->inputs);
    lequal(first->hidden_layers, second->hidden_layers);
    lequal(first->hidden, second->hidden);
    lequal(first->outputs, second->outputs);
    lequal(first->total_weights, second->total_weights);
    int i;
    for (i = 0; i < first->total_weights; ++i) {
        lfequal(first->weight[i], second->weight[i]);
    }
    genann_free(first);
    genann_free(second);
}

void sigmoid() {
    double i = -20;
    const double max = 20;
    const double d = .0001;
    while (i < max) {
        lfequal(genann_act_sigmoid(NULL, i), genann_act_sigmoid_cached(NULL, i));
        i += d;
    }
}

void test_pool()
{
    MemPool *pool = pool_create(128,10);
    char    *cp;
    int     i = 0;
    for ( ; i < 15; ++i )
    {
        char *c = (char*)m_malloc(pool);
        sprintf(c,"%d\tThis Is My Pool\n",i+1);
        printf(c);
    }
    cp = m_malloc(pool);
    m_free(pool,&cp);
    m_free(pool,&cp);
    cp = m_malloc(pool);
    pool_release(pool);
}

void * f1(void *arg)
{
    printf("==================Func 1\n");
    return NULL;
}

void * f2(void* arg)
{
    printf("==================Func 2\n");
    return NULL;
}

void test_malloc()
{
    clock_t       start, end;
    int i , j ;
    start = clock();
    for( j = 0 ;j < runtimes; ++j)
    for ( i = 0 ; i < times; ++i)
    {
       void *f = malloc(bsz);
       free(f);
    }
    end = clock();
    fprintf(stdout,"malloc Time Ellapsed:%d\n",end-start);
}

void test_mempool()
{
    MemPool       *block = pool_create(bsz,1);
    clock_t       start, end;
    int i , j ;
    start = clock();
    for( j = 0 ;j < runtimes; ++j)
    for ( i = 0 ; i < times; ++i)
    {
       void *f = m_malloc(block);
       m_free(block,&f);
    }
    end = clock();
    fprintf(stdout,"Block Pool Time Ellapsed:%d\n",end-start);
    pool_release(block);
}

struct record
{
    const char *precision;
    double lat;
    double lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};

/* Create a bunch of objects as demonstration. */
static int print_preallocated(cJSON *root)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;
    /* formatted print */
    out = cJSON_Print(root);
    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)malloc(len);
    if (buf == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }
    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)malloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }
    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
        printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
            printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
            printf("cJSON_Print result:\n%s\n", out);
            printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }
    /* success */
    printf("%s\n", buf);
    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
        printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
        printf("cJSON_Print result:\n%s\n", out);
        printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }
    free(out);
    free(buf_fail);
    free(buf);
    return 0;
}

/* Create a bunch of objects as demonstration. */
static void create_objects(void)
{
    /* declare a few. */
    cJSON *root = NULL;
    cJSON *fmt = NULL;
    cJSON *img = NULL;
    cJSON *thm = NULL;
    cJSON *fld = NULL;
    int i = 0;
    /* Our "days of the week" array: */
    const char *strings[7] =
    {
        "Sunday",
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday"
    };
    /* Our matrix: */
    int numbers[3][3] =
    {
        {0, -1, 0},
        {1, 0, 0},
        {0 ,0, 1}
    };
    /* Our "gallery" item: */
    int ids[4] = { 116, 943, 234, 38793 };
    /* Our array of "records": */
    struct record fields[2] =
    {
        {
            "zip",
            37.7668,
            -1.223959e+2,
            "",
            "SAN FRANCISCO",
            "CA",
            "94107",
            "US"
        },
        {
            "zip",
            37.371991,
            -1.22026e+2,
            "",
            "SUNNYVALE",
            "CA",
            "94085",
            "US"
        }
    };
    volatile double zero = 0.0;
    /* Here we construct some JSON standards, from the JSON site. */
    /* Our "Video" datatype: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
    cJSON_AddStringToObject(fmt, "type", "rect");
    cJSON_AddNumberToObject(fmt, "width", 1920);
    cJSON_AddNumberToObject(fmt, "height", 1080);
    cJSON_AddFalseToObject (fmt, "interlace");
    cJSON_AddNumberToObject(fmt, "frame rate", 24);
    /* Print to text */
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
    /* Our "days of the week" array: */
    root = cJSON_CreateStringArray(strings, 7);
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
    /* Our matrix: */
    root = cJSON_CreateArray();
    for (i = 0; i < 3; i++)
    {
        cJSON_AddItemToArray(root, cJSON_CreateIntArray(numbers[i], 3));
    }
    /* cJSON_ReplaceItemInArray(root, 1, cJSON_CreateString("Replacement")); */
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
    /* Our "gallery" item: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "Image", img = cJSON_CreateObject());
    cJSON_AddNumberToObject(img, "Width", 800);
    cJSON_AddNumberToObject(img, "Height", 600);
    cJSON_AddStringToObject(img, "Title", "View from 15th Floor");
    cJSON_AddItemToObject(img, "Thumbnail", thm = cJSON_CreateObject());
    cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
    cJSON_AddNumberToObject(thm, "Height", 125);
    cJSON_AddStringToObject(thm, "Width", "100");
    cJSON_AddItemToObject(img, "IDs", cJSON_CreateIntArray(ids, 4));
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
    /* Our array of "records": */
    root = cJSON_CreateArray();
    for (i = 0; i < 2; i++)
    {
        cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", fields[i].address);
        cJSON_AddStringToObject(fld, "City", fields[i].city);
        cJSON_AddStringToObject(fld, "State", fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", fields[i].country);
    }
    /* cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root, 1), "City", cJSON_CreateIntArray(ids, 4)); */
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "number", 1.0 / zero);
    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
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
	//luaL_dofile(L,"a.lua");          //执行整个 Lua 脚本文件 a.lua
	luaL_loadfile(L, "a.lua");
	lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "power");
	lua_pushinteger(L, 2);
	lua_pushinteger(L, 10);
	dump(L);
	lua_call(L, 2, 1);
	//dumpStack(L);
	int res = lua_tonumber(L, -1);  // 取出栈顶结果
	printf("power:%d", res);
	lua_close(L);                   //函数关闭堆栈，释放资源
}

void lua_call_c() {
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	lua_register(L, "add", add);
	luaL_dofile(L, "a.lua");
	lua_close(L);
}

static float randf()
{
	return (float) rand() / ((float) RAND_MAX + 1);
}

static void display(GLFWwindow *window)
{
	glClearColor(randf(), randf(), randf(), 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);
}

static void reshape(GLFWwindow *window, int w, int h)
{
	WINDOW_WIDTH = w > 1 ? w : 1;
	WINDOW_HEIGHT = h > 1 ? h : 1;
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClearDepth(1.0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
}

int main()
{


    #ifndef HAVE_LIBFREETYPE
	/* 2.0.12 */
	fprintf (stderr, "annotate is not useful without freetype.\n"
		"Install freetype, then './configure; make clean; make install'\n"
		"the gd library again.\n"
	);
	return 1;
    #else
	gdImagePtr im;
	char *iin, *iout;
	FILE *in, *out;
	char s[1024];
	int bounds[8];
	int lines = 1;
	int color = gdTrueColor(0, 0, 0);
	char font[1024];
	int size = 12;
	int align = left;
	int x = 0, y = 0;
	char *fontError;

	strcpy(font, "times");

	if(argc != 3) {
		fprintf(stderr, "Usage: annotate imagein.jpg imageout.jpg\n\n");
		fprintf(stderr, "Standard input should consist of\n");
		fprintf(stderr, "lines in the following formats:\n");
		fprintf(stderr, "color r g b (0-255 each) [a (0-127, 0 is opaque)]\n");
		fprintf(stderr, "font fontname\n");
		fprintf(stderr, "size pointsize\n");
		fprintf(stderr, "align (left|right|center)\n");
		fprintf(stderr, "move x y\n");
		fprintf(stderr, "text actual-output-text\n\n");
		fprintf(stderr,
			"If the file 'paris.ttf' exists in /usr/share/fonts/truetype or in a\n");
		fprintf(stderr,
			"location specified in the GDFONTPATH environment variable, 'font paris' is\n");
		fprintf(stderr,
			"sufficient. You may also specify the full, rooted path of a font file.\n");
		exit(1);
	}

	iin = argv[1];
	iout = argv[2];

	in = fopen(iin, "rb");
	if(!in) {
		fprintf(stderr, "Couldn't open %s\n", iin);
		exit(2);
	}

    #ifdef HAVE_LIBJPEG
	im = gdImageCreateFromJpeg(in);
    #else
	fprintf(stderr, "No JPEG library support available.\n");
    #endif
	fclose(in);
	if(!im) {
		fprintf(stderr, "%s did not load properly\n", iin);
		exit(3);
	}
	while(fgets(s, sizeof(s), stdin)) {
		char *st;
		char *text;
		st = strtok(s, " \t\r\n");
		if(!st) {
			/* Be nice about blank lines */
			continue;
		}
		if(!strcmp(st, "font")) {
			char *st = strtok(0, " \t\r\n");
			if(!st) {
				goto badLine;
			}
			strcpy(font, st);
		} else if(!strcmp(st, "align")) {
			char *st = strtok(0, " \t\r\n");

			if(!st) {
				goto badLine;
			}

			if(!strcmp(st, "left")) {
				align = 0;
			} else if(!strcmp(st, "center")) {
				align = 1;
			} else if(!strcmp(st, "right")) {
				align = 2;
			}
		} else if(!strcmp(st, "size")) {
			char *st = strtok(0, " \t\r\n");

			if(!st) {
				goto badLine;
			}

			size = atoi(st);
		} else if(!strcmp(st, "color")) {
			char *st = strtok(0, "\r\n");
			int r, g, b, a = 0;

			if(!st) {
				goto badLine;
			}

			if(sscanf(st, "%d %d %d %d", &r, &g, &b, &a) < 3) {
				fprintf(stderr, "Bad color at line %d\n", lines);
				exit(2);
			}
			color = gdTrueColorAlpha(r, g, b, a);
		} else if(!strcmp(st, "move")) {
			char *st = strtok(0, "\r\n");
			if(!st) {
				goto badLine;
			}
			if(sscanf(st, "%d %d", &x, &y) != 2) {
				fprintf(stderr, "Missing coordinates at line %d\n", lines);
				exit(3);
			}
		} else if(!strcmp(st, "text")) {
			int rx = x;
			text = strtok(0, "\r\n");
			if(!text) {
				text = "";
			}
			gdImageStringFT(0, bounds, color, font, size, 0, x, y, text);
			switch(align) {
				case left:
				break;
				case center:
					rx -= (bounds[2] - bounds[0]) / 2;
				break;
				case right:
					rx -= (bounds[2] - bounds[0]);
				break;
			}
			fontError = gdImageStringFT(im, 0, color, font, size, 0, rx, y, text);
			if(fontError) {
				fprintf(stderr, "font error at line %d: %s\n", lines, fontError);
				exit(7);
			}
			y -= (bounds[7] - bounds[1]);
		} else {
			goto badLine;
		}
		lines++;
		continue;
    badLine:
		fprintf(stderr, "Bad syntax, line %d\n", lines);
		exit(4);
	}
	out = fopen(iout, "wb");
	if(!out) {
		fprintf(stderr, "Cannot create %s\n", iout);
		exit(5);
	}
    #ifdef HAVE_LIBJPEG
	gdImageJpeg(im, out, 95);
    #else
	fprintf(stderr, "No JPEG library support available.\n");
    #endif
	gdImageDestroy(im);
	fclose(out);
    #endif /* HAVE_LIBFREETYPE */

 	lua_State* L = luaL_newstate();
    luaL_openlibs( L );
    luaL_register( L, LIB_NAME, bindFuncs);
    luaopen_crypt(L);
    luaopen_lfs(L);
    luaopen_lsqlite3(L);
    luaopen_socket_core(L);
    luaopen_luagl(L);
    luaopen_luaglu(L);
    luaopen_xml_core(L);
    open_file( L, "init.lua" );
    lua_close( L );
	getchar();


    ini_t *config = ini_load("config.ini");
    const char *name = ini_get(config, "owner", "name");
    if (name) {
        printf("name: %s\n", name);
    }
    const char *server = "default";
    int port = 80;
    ini_sget(config, "database", "server", NULL, &server);
    ini_sget(config, "database", "port", "%d", &port);
    printf("server: %s:%d\n", server, port);
    ini_free(config);
	getchar();


	printf("GENANN TEST SUITE\n");
    srand(100); //Repeatable test results.
    lrun("basic", basic);
    lrun("xor", xor);
    lrun("backprop", backprop);
    lrun("train and", train_and);
    lrun("train or", train_or);
    lrun("train xor", train_xor);
    lrun("persist", persist);
    lrun("copy", copy);
    lrun("sigmoid", sigmoid);
    lresults();
    getchar();

    test_pool();
    test_mempool();
    test_malloc();
    getchar();

    printf("Version: %s\n", cJSON_Version());
    create_objects();

    LoggerInit();
    printf("\nERROR, WARNING, SUCCESS LOGS\n");
    printf("========\n");
    LoggerSetLevel(LOGGER_LEVEL_ERROR | LOGGER_LEVEL_WARNING |  LOGGER_LEVEL_SUCCESS);
    LoggerError("Error one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerWarning("warning one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerSuccess("success one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerInfo("Info one : %d two : %f three : %s", 1, 2.0, "three");
    LoggerDebug("Debug one : %d two : %f three : %s", 1, 2.0, "three");
    printf("\nALL LOGS\n");
    printf("========\n");
    LoggerSetLevel(LOGGER_LEVEL_ALL);
    LoggerError("assets path not found %s", "../assets/image.png");
    LoggerWarning("retry fetch, network error");
    LoggerSuccess("data fetched %dms", 230);
    LoggerInfo("init queue");
    LoggerDebug("user email is : %s", "user@test.test");
    LoggerClose();
    getchar();

    c_call_lua();
    getchar();

	lua_call_c();
    getchar();


    char* lua = "local base = _G \nlocal http=require('http')\nlocal b,c,h = http.request('http://blog.csdn.net/');\nbase.print(b);";
    printf("LUA CODE:\n%s\n", lua);
    getchar();

    GLFWwindow *window;
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "cookie", NULL, NULL);
	glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetWindowRefreshCallback(window, display);
	glfwMakeContextCurrent(window);
	if (gl3wInit()) {
		fprintf(stderr, "failed to initialize OpenGL\n");
		return -1;
	}
	if (!gl3wIsSupported(3, 2)) {
		fprintf(stderr, "OpenGL 3.2 not supported\n");
		return -1;
	}
	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
	while (!glfwWindowShouldClose(window)) {
		display(window);
		glfwPollEvents();
	}
	glfwTerminate();
    getchar();
	return lfails != 0;
}
