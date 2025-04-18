/*
* Copyright 2022 Rochus Keller <mailto:me@rochus-keller.ch>
*
* This file is part of the BUSY build system.
*
* The following is the license that applies to this copy of the
* application. For a license to use the application under conditions
* other than those described here, please email to me@rochus-keller.ch.
*
* GNU General Public License Usage
* This file may be used under the terms of the GNU General Public
* License (GPL) versions 2.0 or 3.0 as published by the Free Software
* Foundation and appearing in the file LICENSE.GPL included in
* the packaging of this file. Please review the following information
* to ensure GNU General Public Licensing requirements will be met:
* http://www.fsf.org/licensing/licenses/info/GPLv2.html and
* http://www.gnu.org/copyleft/gpl.html.
*/

#include "bslib.h"
#include "lauxlib.h"
#include "bslex.h"
#include "bsparser.h"
#include "bsrunner.h"
#include "bshost.h"
#include "bsunicode.h"
#include <ctype.h>
#include <string.h>
#include <assert.h>

static int bs_version(lua_State *L)
{
    lua_pushstring(L,BS_BSVERSION);
    return 1;
}

// returns: current directory in normalized form
static int bs_getcwd (lua_State *L)
{
    if( bs_cwd() == BS_OK )
        lua_pushstring(L, bs_global_buffer());
    else
        luaL_error(L,"getcwd: received non supported path from OS");
    return 1;
}

static int host_cpu(lua_State *L)
{
    BSCpu c = bs_host_cpu();
    lua_pushstring(L,c.name);
    lua_pushinteger(L,c.ver);
    return 2;
}

static int host_os(lua_State *L)
{
    lua_pushstring(L,bs_host_os());
    return 1;
}

static int host_wordsize(lua_State *L)
{
    lua_pushinteger(L,bs_wordsize());
    return 1;
}

static int host_compiler(lua_State *L)
{
    BSCompiler c = bs_host_compiler();
    lua_pushstring(L,c.name);
    lua_pushinteger(L,c.ver);
    return 2;
}


// returns: file path of the currently running application in normalized form
static int thisapp (lua_State *L)
{
    return bs_thisapp2(L);
}

static void push_normalized(lua_State *L, int path)
{
    int res = bs_normalize_path2(lua_tostring(L,path));
    switch(res)
    {
    case BS_OK:
        break;
    case BS_NotSupported:
        luaL_error(L,"path format is not supported: %s", lua_tostring(L,path) );
        break;
    case BS_InvalidFormat:
        luaL_error(L,"path format is invalid: %s", lua_tostring(L,path) );
        break;
    case BS_OutOfSpace:
        luaL_error(L,"path is too long to be handled: %s", lua_tostring(L,path) );
        break;
    default:
        luaL_error(L,"unknown error for path: %s", lua_tostring(L,path) );
        break;
    }

    lua_pushstring(L, bs_global_buffer());
    if( strncmp(bs_global_buffer(),"//",2) != 0 )
    {
        // relative path
        res = bs_cwd();
        if( res != BS_OK )
            luaL_error(L,"getcwd delivered a path not supported by this application" );
        lua_pushstring(L,bs_global_buffer());
        res = bs_add_path(L,-1,-2);
        if( res != 0 )
            luaL_error(L,"creating absolute path from provided root gives an error: %s", lua_tostring(L,1) );
        // stack: rhs, lhs, abspath
        lua_replace(L,-3);
        lua_pop(L,1);
        // stack: abspath
    }
}

// TODO: an optional way to set default config values as supported in GN with set_defaults() and the build config
// file; see 'gn help execution'

// opt param: path to source root directory (default '..')
// opt param: path to output root dir (default ./output)
// opt param: table with parameter values
// NOTE: if BUSY is built information about the OS and toolchain is melted into the executable and doesn't have
//   to be explicitly set when BUSY is used.
// returns: root module
static int bs_compile (lua_State *L)
{
    int i;
    for( i = lua_gettop(L); i < 3; i++ )
        lua_pushnil(L);
    if( lua_isnil(L,1) )
    {
        lua_pushstring(L, "..");
        lua_replace(L,1);
    }
    if( lua_isnil(L,2) )
    {
        lua_pushstring(L, "./output");
        lua_replace(L,2);
    }
    if( lua_isnil(L,3) )
    {
        lua_createtable(L,0,0);
        lua_replace(L,3);
    }

    // set lua path so no environment can intervene
    lua_getglobal(L, "package");
    lua_pushstring(L,"./?.lua");
    lua_setfield(L,-2,"path");
    lua_pushstring(L,"");
    lua_setfield(L,-2,"cpath");
    lua_pop(L,1);

    lua_getglobal(L, "require");
    lua_pushstring(L, "builtins");
    lua_call(L,1,1);
    lua_getfield(L,-1,"#inst");
    push_normalized(L,2);
    lua_setfield(L,-2,"root_build_dir");
    push_normalized(L,1);
    fprintf(stdout,"# running parser\n# root source directory is %s\n",lua_tostring(L,-1));
    fflush(stdout);
    lua_setfield(L,-2,"root_source_dir");
    lua_pop(L,2);

    lua_pushcfunction(L, bs_parse);

    push_normalized(L,1);

    lua_pushnil(L);

    lua_pushvalue(L,3);

    lua_call(L,3,1);
    // module is on the stack
    lua_pushvalue(L,-1);
    lua_setglobal(L, "#root");

    lua_pushnil(L);
    while (lua_next(L, 3) != 0)
    {
        luaL_error(L,"cannot set unknown parameter: %s", lua_tostring(L,-2) );
        lua_pop(L, 1);
    }

    return 1;
}

static int resolvedesig(lua_State *L, const char* desig, int root)
{
    const char* p = desig;
    const char* q = desig;
    lua_pushvalue(L,root);
    const int module = lua_gettop(L);
    while( 1 )
    {
        uchar len = 0;
        const uint ch = unicode_decode_utf8((const uchar*)q,&len);
        if( len == 0 )
            luaL_error(L,"the passed-in product designator has invalid UTF-8 format: %s",desig);
        if( ch == 0 || ch == '.' )
        {
            if( p == q )
                luaL_error(L,"the passed-in product designator has invalid syntax: %s",desig);
            if( p != desig )
            {
                if( !lua_istable(L,-1) )
                {
                    lua_pushlstring(L,desig,p-desig-1);
                    luaL_error(L,"'%s' of passed-in designator '%s' cannot be dereferenced",lua_tostring(L,-1),desig);
                }
                assert(lua_istable(L,-1));
                lua_getfield(L,-1,"#kind");
                const int k = lua_tointeger(L,-1);
                lua_pop(L,1);
                if( k != BS_ModuleDef )
                {
                    lua_pushlstring(L,desig,p-desig-1);
                    luaL_error(L,"'%s' of passed-in designator '%s' must be a subdir declaration",lua_tostring(L,-1),desig);
                }
                lua_getfield(L,-1,"#visi");
                const int v = lua_tointeger(L,-1);
                lua_pop(L,1);
                if( v != BS_Public )
                {
                    lua_pushlstring(L,desig,p-desig-1);
                    luaL_error(L,"subdir '%s' of passed-in designator '%s' is not public",lua_tostring(L,-1),desig);
                }
            }
            lua_pushlstring(L,p,q-p);
            lua_rawget(L,module);
            if( lua_isnil(L,-1) )
            {
                lua_pushlstring(L,p,q-p);
                luaL_error(L,"identifier '%s' of passed-in designator '%s' not found",lua_tostring(L,-1),desig);
            }
            p = q + 1;
            if( ch == '.' && *p == 0 )
                luaL_error(L,"the passed-in product designator has invalid syntax: %s",desig);
            lua_replace(L,module);
        }
        if( ch == 0 )
            break;
        q += len;
    }
    return 1; // returns found decl or doesn't return
}

static int isproductvardecl(lua_State *L, int t, int bi)
{
    if( t <= 0 )
        t += lua_gettop(L) + 1;
    if( !lua_istable(L,t) )
        return 0;
    lua_getfield(L,t,"#kind");
    const int k = lua_tointeger(L,-1);
    lua_pop(L,1);
    if( k != BS_VarDecl )
        return 0;
    lua_getfield(L,t,"#type");
    lua_getfield(L,bi,"Product");
    const int res = bs_isa(L,-1,-2);
    lua_pop(L,2);
    return res;
}

static int fetchInstOfDecl(lua_State* L, int decl)
{
    const int top = lua_gettop(L);
    if( decl < 0 )
        decl += top + 1;
    lua_getfield(L,decl,"#owner");
    lua_getfield(L,-1,"#inst");
    lua_replace(L,-2);
    // stack: modinst
    lua_getfield(L,decl,"#name");
    lua_rawget(L,-2);
    // stack: modinst, classinst
    lua_replace(L,-2);
    assert(top + 1 == lua_gettop(L) );
    return 1;
}

// param: root module def
// opt param: set of product desigs to be built
static int bs_execute (lua_State *L)
{
    enum { ROOT = 1, PRODS };

    if( !lua_istable(L,ROOT) )
        luaL_error(L,"expecting a module definition");
    lua_getfield(L,ROOT,"#kind");
    if( lua_tointeger(L,-1) != BS_ModuleDef )
        luaL_error(L,"expecting a module definition");

    lua_getglobal(L, "require");
    lua_pushstring(L, "builtins");
    lua_call(L,1,1);
    const int builtins = lua_gettop(L);

    lua_getfield(L,ROOT,"#dir");
    fprintf(stdout,"# running build for %s\n",lua_tostring(L,-1));
    lua_getfield(L,builtins,"#inst");
    lua_getfield(L,-1,"root_build_dir");
    const int rootPath = lua_gettop(L);
    fprintf(stdout,"# root build directory is %s\n",lua_tostring(L,-1));
    fflush(stdout);

    // we create build dir tree here because target dependencies can skip directories and cause
    // mkdir errors if levels are (temporarily) missing.
    lua_pushcfunction(L, bs_createBuildDirs);
    lua_pushvalue(L,ROOT);
    lua_pushvalue(L,rootPath);
    lua_call(L,2,0);

    lua_pop(L,3); // dir, inst, build_dir

    if( lua_isnil(L,PRODS) )
    {
        int foundAny = 0;
        // search for default products an run them
        const int n = lua_objlen(L,ROOT);
        int i;
        for( i = 1; i <= n; i++ )
        {
            lua_rawgeti(L,ROOT,i);
            const int hit = isproductvardecl(L,-1,builtins);
            if( hit )
            {
                lua_getfield(L,-1,"#visi");
                const int visi = lua_tointeger(L,-1);
                lua_pop(L,1);
                if( visi == BS_PublicDefault )
                {
                    foundAny = 1;
                    lua_pushcfunction(L, bs_run);
                    fetchInstOfDecl(L,-2);
                    lua_call(L,1,0);
                }
            }
            lua_pop(L,1);
        }
        if( !foundAny )
            luaL_error(L,"the module doesn't have any default product declarations");
    }else
    {
        // build all products in the set; error if not visible
        lua_pushnil(L);
        while (lua_next(L, PRODS) != 0)
        {
            resolvedesig(L,lua_tostring(L,-2),ROOT);
            const int hit = isproductvardecl(L,-1,builtins);
            if( hit )
            {
                lua_getfield(L,-1,"#visi");
                const int visi = lua_tointeger(L,-1);
                lua_pop(L,1);
                if( visi >= BS_Public )
                {
                    lua_pushcfunction(L, bs_run);
                    fetchInstOfDecl(L,-2);
                    lua_call(L,1,0);
                }else
                    luaL_error(L,"the declaration is not visible from outside: %s", lua_tostring(L,-3));
            }else
                luaL_error(L,"no valid product declaration: %s", lua_tostring(L,-3));
            lua_pop(L,2);
        }
    }
    return 0;
}

static const luaL_Reg bslib[] = {
    {"compile",      bs_compile},
    {"execute",      bs_execute},
    {"dump",      bs_dump},
    {"getcwd",      bs_getcwd},
    {"thisapp",      thisapp},
    {"run", bs_run},
    {"cpu", host_cpu },
    {"os", host_os },
    {"wordsize", host_wordsize },
    {"compiler", host_compiler },
    {"version", bs_version },
    {NULL, NULL}
};

LUALIB_API int bs_open_busy (lua_State *L) {
  luaL_register(L, BS_BSLIBNAME, bslib);
  return 1;
}
