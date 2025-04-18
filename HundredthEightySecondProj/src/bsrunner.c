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

#include "bsrunner.h"
#include "bshost.h"
#include "bsparser.h" 
#include "lauxlib.h"
#include <assert.h>
#include <string.h>

static int calcdesig(lua_State* L, int decl)
{
    if( decl < 0 )
        decl += lua_gettop(L) + 1;
    lua_getfield(L,decl,"#name");
    const int name = lua_gettop(L);
    lua_getfield(L,decl,"#owner");
    const int module = lua_gettop(L);
    while(!lua_isnil(L,module))
    {
        lua_getfield(L,module,"#name");
        if( lua_isnil(L,-1) )
        {
            lua_pop(L,1);
            break;
        }
        lua_pushstring(L,".");
        lua_pushvalue(L,name);
        lua_concat(L,3);
        lua_replace(L,name);
        lua_getfield(L,module,"#owner");
        lua_replace(L,module);
    }
    lua_pop(L,1); // module
    return 1;
}

static int isa(lua_State* L, int builtins, int cls, const char* what )
{
    lua_getfield(L,builtins,what);
    const int res = bs_isa(L,-1,cls);
    lua_pop(L,1);
    return res;
}

typedef enum BSToolchain {BS_msvc,BS_gcc,BS_clang} BSToolchain;
typedef enum BSLanguage { BS_unknownLang, BS_c, BS_cc, BS_objc, BS_objcc, BS_header } BSLanguage;

static int guessLang(const char* name)
{
    const int len = strlen(name);
    const char* p = name + len - 1;
    while( p != name && *p != '.' )
        p--;

    // see https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Overall-Options.html
    if( strcmp(p,".c") == 0 )
        return BS_c;
    if( strcmp(p,".h") == 0 )
        return BS_header;
    if( strcmp(p,".cc") == 0 )
        return BS_cc;
    if( strcmp(p,".hh") == 0 )
        return BS_header;
#ifndef _WIN32
    if( strcmp(p,".C") == 0 )
        return BS_cc;
    if( strcmp(p,".H") == 0 )
        return BS_header;
    if( strcmp(p,".HPP") == 0 )
        return BS_header;
    if( strcmp(p,".M") == 0 )
        return BS_objcc;
#endif
    if( strcmp(p,".cpp") == 0 )
        return BS_cc;
    if( strcmp(p,".hpp") == 0 )
        return BS_header;
    if( strcmp(p,".c++") == 0 )
        return BS_cc;
    if( strcmp(p,".h++") == 0 )
        return BS_header;
    if( strcmp(p,".cp") == 0 )
        return BS_cc;
    if( strcmp(p,".hp") == 0 )
        return BS_header;
    if( strcmp(p,".cxx") == 0 )
        return BS_cc;
    if( strcmp(p,".hxx") == 0 )
        return BS_header;
    if( strcmp(p,".m") == 0 )
        return BS_objc;
    if( strcmp(p,".mm") == 0 )
        return BS_objcc;

    return BS_unknownLang;
}

static void addflags(lua_State* L, int list, int out)
{
    if( list < 0 )
        list += lua_gettop(L) + 1;

    const int n = lua_objlen(L,list);
    int i;
    for( i = 1; i <= n; i++ )
    {
        lua_pushvalue(L,out);
        lua_pushstring(L," ");
        lua_rawgeti(L,list,i);
        lua_concat(L,3);
        lua_replace(L,out);
    }
}

static void addPath(lua_State* L, int lhs, int rhs)
{
    if( *lua_tostring(L,rhs) == '/' )
        lua_pushvalue(L, rhs);
    else if( bs_add_path(L,lhs,rhs) )
        luaL_error(L,"creating absolute path from provided root gives an error: %s %s",
                   lua_tostring(L,lhs), lua_tostring(L,rhs) );
}

static int getModuleVarFrom(lua_State* L, int inst, const char* name )
{
    const int top = lua_gettop(L);

    lua_getfield(L,inst,"#decl");
    lua_getfield(L,-1,"#owner");
    lua_replace(L,-2);
    lua_getfield(L,-1,name);
    lua_replace(L,-2);

    const int bottom = lua_gettop(L);
    assert( top + 1 == bottom );
    return 1;
}

static void addall(lua_State* L,int inst,int cflags, int cflags_c, int cflags_cc, int cflags_objc, int cflags_objcc,
                        int defines, int includes, int ismsvc)
{
    lua_getfield(L,inst,"configs");
    const int configs = lua_gettop(L);
    size_t i;
    for( i = 1; i <= lua_objlen(L,configs); i++ )
    {
        lua_rawgeti(L,configs,i);
        const int config = lua_gettop(L);
        // TODO: check for circular deps
        addall(L,config,cflags,cflags_c,cflags_cc,cflags_objc,cflags_objcc,defines,includes,ismsvc);
        lua_pop(L,1); // config
    }
    lua_pop(L,1); // configs

    lua_getfield(L,inst,"cflags");
    addflags(L,-1,cflags);
    lua_pop(L,1);

    lua_getfield(L,inst,"cflags_c");
    addflags(L,-1,cflags_c);
    lua_pop(L,1);

    lua_getfield(L,inst,"cflags_cc");
    addflags(L,-1,cflags_cc);
    lua_pop(L,1);

    lua_getfield(L,inst,"cflags_objc");
    addflags(L,-1,cflags_objc);
    lua_pop(L,1);

    lua_getfield(L,inst,"cflags_objcc");
    addflags(L,-1,cflags_objcc);
    lua_pop(L,1);

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);
    const char* s = lua_tostring(L,absDir);

    lua_getfield(L,inst,"include_dirs");
    const int incls = lua_gettop(L);

    for( i = 1; i <= lua_objlen(L,incls); i++ )
    {
        lua_rawgeti(L,incls,i);
        const int path = lua_gettop(L);
        if( *lua_tostring(L,-1) != '/' )
        {
            // relative path
            addPath(L,absDir,path);
            lua_replace(L,path);
        }
        lua_pushvalue(L,includes);
        lua_pushfstring(L," -I\"%s\" ", bs_denormalize_path(lua_tostring(L,path)) );
        lua_concat(L,2);
        lua_replace(L,includes);
        lua_pop(L,1); // path
    }
    lua_pop(L,2); // absDir, incls

    lua_getfield(L,inst,"defines");
    const int defs = lua_gettop(L);
    for( i = 1; i <= lua_objlen(L,defs); i++ )
    {
        lua_rawgeti(L,defs,i);
        lua_pushvalue(L,defines);
        if( strstr(lua_tostring(L,-2),"\\\"") != NULL )
            lua_pushfstring(L," \"-D%s\" ", lua_tostring(L,-2)); // strings can potentially include whitespace, thus quotes
        else
            lua_pushfstring(L," -D%s ", lua_tostring(L,-2));
        lua_concat(L,2);
        lua_replace(L,defines);
        lua_pop(L,1); // def
    }
    lua_pop(L,1); // defs

}

static int getToolchain(lua_State* L, int builtinsInst)
{
    lua_getfield(L,builtinsInst,"target_toolchain");
    int toolchain;
    if( strcmp(lua_tostring(L,-1),"msvc") == 0 )
        toolchain = BS_msvc;
    else if( strcmp(lua_tostring(L,-1),"gcc") == 0 )
        toolchain = BS_gcc;
    else if( strcmp(lua_tostring(L,-1),"clang") == 0 )
        toolchain = BS_clang;
    else
        luaL_error(L,"toolchain not supported: %s",lua_tostring(L,-1) );
    lua_pop(L,1);
    return toolchain;
}

typedef enum BSOutKind { // #kind
    BS_Nothing,
    BS_Mixed, // list of list of the other kinds
    BS_ObjectFiles,
    BS_StaticLib,
    BS_DynamicLib,
    BS_Executable,
    BS_SourceFiles // in case of Moc
} BSOutKind;

static void copyItems(lua_State* L, int inlist, int outlist, BSOutKind what )
{
    lua_getfield(L,inlist,"#kind");
    const unsigned k = lua_tointeger(L,-1);
    lua_pop(L,1); // kind

    if( k == BS_Mixed )
    {
        size_t i, len = lua_objlen(L,inlist);
        for( i = 1; i <= len; i++ )
        {
            lua_rawgeti(L,inlist,i);
            const int sublist = lua_gettop(L);
            copyItems(L,sublist,outlist,what);
            lua_pop(L,1); // sublist
        }
    }else if( k == what )
    {
        // also works with BS_DynamicLib, BS_StaticLib and BS_Executable
        size_t i, len = lua_objlen(L,inlist);
        for( i = 1; i <= len; i++ )
        {
            lua_rawgeti(L,inlist,i);
            lua_rawseti(L,outlist, lua_objlen(L,outlist)+1 );
        }
    }
}

static void compilesources(lua_State* L, int inst, int builtins, int inlist)
{
    const int top = lua_gettop(L);

    lua_createtable(L,0,0);
    const int outlist = lua_gettop(L);
    lua_pushinteger(L,BS_ObjectFiles);
    lua_setfield(L,outlist,"#kind");
    lua_pushvalue(L,outlist);
    lua_setfield(L,inst,"#out");

    lua_getfield(L,builtins,"#inst");
    const int binst = lua_gettop(L);

    const int toolchain = getToolchain(L,binst);

    lua_getfield(L,binst,"root_build_dir");
    const int rootOutDir = lua_gettop(L);

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);

    getModuleVarFrom(L,inst,"#rdir");
    const int relDir = lua_gettop(L);

    lua_pushstring(L,"");
    const int cflags = lua_gettop(L);
    lua_pushstring(L,"");
    const int cflags_c = lua_gettop(L);
    lua_pushstring(L,"");
    const int cflags_cc = lua_gettop(L);
    lua_pushstring(L,"");
    const int cflags_objc = lua_gettop(L);
    lua_pushstring(L,"");
    const int cflags_objcc = lua_gettop(L);
    lua_pushstring(L,"");
    const int defines = lua_gettop(L);
    lua_pushstring(L,"");
    const int includes = lua_gettop(L);

    addall(L,inst,cflags,cflags_c,cflags_cc,cflags_objc,cflags_objcc,defines,includes,toolchain == BS_msvc);

    size_t i;

    lua_getfield(L,inst,"sources");
    const int sources = lua_gettop(L);
    lua_createtable(L,lua_objlen(L,sources),0);
    for( i = 1; i <= lua_objlen(L,sources); i++ )
    {
        // copy all sources to a new table to avoid changing the original sources
        lua_rawgeti(L,sources,i);
        lua_rawseti(L,-2,i);
    }
    lua_replace(L,-2);
    copyItems(L,inlist,sources, BS_SourceFiles);
    copyItems(L,inlist,outlist, BS_ObjectFiles);

    int n = lua_objlen(L,outlist);
    for( i = 1; i <= lua_objlen(L,sources); i++ )
    {
        lua_rawgeti(L,sources,i);
        const int file = lua_gettop(L);
        const int lang = guessLang(lua_tostring(L,file));
        if( lang == BS_unknownLang )
            luaL_error(L,"source file type not supported: %s",lua_tostring(L,file));
        if( lang == BS_header )
        {
            lua_pop(L,1);
            continue;
        }

#if 0
        FILE* tmp = fopen("files.txt","a");
        fwrite(lua_tostring(L,file),1,lua_objlen(L,file),tmp);
        fwrite("\n",1,1,tmp);
        fclose(tmp);
#endif

        if( *lua_tostring(L,file) != '/' )
            addPath(L,absDir,file); // path could be absolute!
        else
            lua_pushvalue(L,file);
        const int src = lua_gettop(L);

        addPath(L,rootOutDir,relDir);
        // only use the filename and suffix here, strip all path segments; otherwise subdirs have to be created;
#ifdef BS_HAVE_FILE_PREFIX
        // the name is actually not relevant, so we can just prefix it with a number to reduce name collisions
        // (possible when collecting  files from different directories in the same module)
        lua_pushfstring(L,"/%d_%s",i,bs_filename(lua_tostring(L,file)));
#else
        lua_pushfstring(L,"/%s",bs_filename(lua_tostring(L,file)));
#endif
        if( toolchain == BS_msvc )
            lua_pushstring(L,".obj");
        else
            lua_pushstring(L,".o");
        lua_concat(L,3);
        const int out = lua_gettop(L);

        lua_pushvalue(L,out);
        lua_rawseti(L,outlist,++n);

        const time_t srcExists = bs_exists(lua_tostring(L,src));
        const time_t outExists = bs_exists(lua_tostring(L,out));
        // check if out is older than source; this is just to avoid total recompile in case of an error,
        // not for development
        if( !outExists || outExists < srcExists )
        {
            switch(toolchain)
            {
            case BS_gcc:
                lua_pushstring(L,"gcc ");
                break;
            case BS_clang:
                lua_pushstring(L,"clang ");
                break;
            case BS_msvc:
                lua_pushstring(L,"cl ");
                break;
            }
            const int cmd = lua_gettop(L);
            lua_pushvalue(L,cmd);
            lua_pushvalue(L,cflags);
            lua_pushvalue(L,includes);
            lua_pushvalue(L,defines);
            switch(lang)
            {
            case BS_c:
                lua_pushstring(L,"");
                break;
            case BS_cc:
                lua_pushvalue(L,cflags_cc);
                break;
            case BS_objc:
                lua_pushvalue(L,cflags_objc);
                break;
            case BS_objcc:
                lua_pushvalue(L,cflags_objcc);
                break;
            }
            switch(toolchain)
            {
            case BS_gcc:
            case BS_clang:
                lua_pushstring(L," -O2 -c -o ");
                lua_pushfstring(L,"\"%s\" ", bs_denormalize_path(lua_tostring(L,out) ) );
                lua_pushfstring(L,"\"%s\" ", bs_denormalize_path(lua_tostring(L,src) ) );
                break;
            case BS_msvc:
                lua_pushstring(L," /nologo /O2 /MD /c /Fo");
                lua_pushfstring(L,"\"%s\" ", bs_denormalize_path(lua_tostring(L,out) ) );
                lua_pushfstring(L,"\"%s\" ", bs_denormalize_path(lua_tostring(L,src) ) );
                break;
            }
            lua_concat(L,8);
            lua_replace(L,cmd);
            fprintf(stdout,"%s\n", lua_tostring(L,cmd));
            fflush(stdout);
            if( bs_exec(lua_tostring(L,cmd)) != 0 ) // works for all gcc, clang and cl
            {
                // stderr was already written to the console
                lua_pushnil(L);
                lua_error(L);
            }
            lua_pop(L,1); // cmd
        }
        lua_pop(L,3); // file, source, dest
    }
    lua_pop(L,1); // sources

    lua_pop(L,12); // outlist, binst, rootOutDir...relDir, cflags...includes

    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static void addall2(lua_State* L,int inst,int ldflags, int lib_dirs, int lib_names,
                    int lib_files, int frameworks, int ismsvc, int ismac, int iswin)
{
    const int top = lua_gettop(L);

    lua_getfield(L,inst,"configs");
    const int configs = lua_gettop(L);
    size_t i;
    for( i = 1; i <= lua_objlen(L,configs); i++ )
    {
        lua_rawgeti(L,configs,i);
        const int conf = lua_gettop(L);
        // TODO: check for circular deps
        addall2(L,conf,ldflags,lib_dirs,lib_names,lib_files,frameworks, ismsvc, ismac, iswin);
        lua_pop(L,1);
    }
    lua_pop(L,1); // configs

    lua_getfield(L,inst,"ldflags");
    addflags(L,-1,ldflags);
    lua_pop(L,1);

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);

    lua_getfield(L,inst,"lib_dirs");
    const int ldirs = lua_gettop(L);

    for( i = 1; i <= lua_objlen(L,ldirs); i++ )
    {
        lua_rawgeti(L,ldirs,i);
        const int path = lua_gettop(L);
        if( *lua_tostring(L,-1) != '/' )
        {
            // relative path
            addPath(L,absDir,path);
            lua_replace(L,path);
        }
        lua_pushvalue(L,lib_dirs);
        if(ismsvc)
            lua_pushfstring(L," /libpath:\"%s\" ", bs_denormalize_path(lua_tostring(L,path)) );
        else
            lua_pushfstring(L," -L\"%s\" ", bs_denormalize_path(lua_tostring(L,path)) );
        lua_concat(L,2);
        lua_replace(L,lib_dirs);
        lua_pop(L,1); // path
    }
    lua_pop(L,1); // ldirs


    lua_getfield(L,inst,"lib_names");
    const int lnames = lua_gettop(L);
    for( i = 1; i <= lua_objlen(L,lnames); i++ )
    {
        lua_rawgeti(L,lnames,i);
        lua_pushvalue(L,lib_names);
        if(ismsvc)
            lua_pushfstring(L," %s.lib ", lua_tostring(L,-2));
        else
            lua_pushfstring(L," -l%s ", lua_tostring(L,-2));
        lua_concat(L,2);
        lua_replace(L,lib_names);
        lua_pop(L,1); // name
    }
    lua_pop(L,1); // lnames

    if(ismac)
    {
        lua_getfield(L,inst,"frameworks");
        const int fw = lua_gettop(L);
        for( i = 1; i <= lua_objlen(L,lnames); i++ )
        {
            lua_rawgeti(L,fw,i);
            lua_pushvalue(L,frameworks);
            lua_pushfstring(L," -framework %s ", lua_tostring(L,-2));
            lua_concat(L,2);
            lua_replace(L,frameworks);
            lua_pop(L,1); // name
        }
        lua_pop(L,1); // fw
    }

    if( iswin )
    {
        lua_getfield(L,inst,"def_file");
        const int def_file = lua_gettop(L);
        if( !lua_isnil(L,-1) && strcmp(lua_tostring(L,-1),".") != 0 )
        {
            lua_pushvalue(L,ldflags);
            lua_pushstring(L," ");
            if( *lua_tostring(L,def_file) != '/' )
            {
                // relative path
                addPath(L,absDir,def_file);
                lua_replace(L,def_file);
            }
            if(ismsvc)
                lua_pushfstring(L," /def:\"%s\" ", bs_denormalize_path(lua_tostring(L,def_file)) );
            else
                lua_pushfstring(L," \"%s\" ", bs_denormalize_path(lua_tostring(L,def_file)) );
            lua_concat(L,3);
            lua_replace(L,ldflags);
        }
        lua_pop(L,1);// def_file
    }

    //  TODO: lib_files

    lua_pop(L,1); // absDir
    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static time_t renderobjectfiles(lua_State* L, int list, FILE* out, int buf, int ismsvc)
{
    // BS_ObjectFiles: list of file names
    // BS_StaticLib, BS_DynamicLib, BS_Executable: one file name
    // BS_Mixed: list of tables

    lua_getfield(L,list,"#kind");
    const int k = lua_tointeger(L,-1);
    lua_pop(L,1); // kind

    time_t srcExists = 0;

    size_t i;
    switch(k)
    {
    case BS_Mixed:
        for( i = lua_objlen(L,list); i >= 1; i-- )
        {
            lua_rawgeti(L,list,i);
            const int sublist = lua_gettop(L);
            assert( lua_istable(L,sublist) );
            const time_t exists = renderobjectfiles(L,sublist,out, buf, ismsvc);
            if( exists > srcExists )
                srcExists = exists;
            lua_pop(L,1); // sublist
        }
        break;
    case BS_ObjectFiles:
        for( i = lua_objlen(L,list); i >= 1; i-- )
        {
            lua_rawgeti(L,list,i);
            const int path = lua_gettop(L);
            const time_t exists = bs_exists(lua_tostring(L,path));
            if( exists > srcExists )
                srcExists = exists;
            if( buf )
            {
                lua_pushvalue(L,buf);
                lua_pushfstring(L,"\"%s\" ", bs_denormalize_path(lua_tostring(L,path)) );
                lua_concat(L,2);
                lua_replace(L,buf);
            }else
                fprintf(out,"\"%s\" ", bs_denormalize_path(lua_tostring(L,path)) );
            lua_pop(L,1); // path
        }
        break;
    case BS_StaticLib:
    case BS_DynamicLib:
        {
            lua_rawgeti(L,list,1); // there is only one item, which has index 1
            const int path = lua_gettop(L);

            if(ismsvc && k == BS_DynamicLib)
            {
                // add .lib because msvc requires an import library to use the dll
                lua_pushstring(L,".lib");
                lua_concat(L,2); // the name of the import library is xyz.dll.lib
            }

            const time_t exists = bs_exists(lua_tostring(L,path));
            if( exists > srcExists )
                srcExists = exists;

            if( buf )
            {
                lua_pushvalue(L,buf);
                lua_pushfstring(L,"\"%s\" ", bs_denormalize_path(lua_tostring(L,path)) );
                lua_concat(L,2);
                lua_replace(L,buf);
            }else
                fprintf(out,"\"%s\" ", bs_denormalize_path(lua_tostring(L,path)) );
            lua_pop(L,1); // path
        }
        break;
    default:
        // ignore
        break;
    }
    return srcExists;
}

static void link(lua_State* L, int inst, int builtins, int inlist, int kind)
{
    assert( kind == BS_Executable || kind == BS_DynamicLib || kind == BS_StaticLib );

    const int top = lua_gettop(L);

    lua_getfield(L,builtins,"#inst");
    const int binst = lua_gettop(L);

    const int toolchain = getToolchain(L,binst);
    lua_getfield(L,binst,"target_os");
    const int win32 = strcmp(lua_tostring(L,-1),"win32") == 0 || strcmp(lua_tostring(L,-1),"winrt") == 0;
    const int mac = strcmp(lua_tostring(L,-1),"darwin") == 0 || strcmp(lua_tostring(L,-1),"macos") == 0;
    lua_pop(L,1);


    lua_getfield(L,binst,"root_build_dir");
    const int rootOutDir = lua_gettop(L);

    getModuleVarFrom(L,inst,"#rdir");
    const int relDir = lua_gettop(L);

    lua_pushstring(L,"");
    const int ldflags = lua_gettop(L);
    lua_pushstring(L,"");
    const int lib_dirs = lua_gettop(L);
    lua_pushstring(L,"");
    const int lib_names = lua_gettop(L);
    lua_pushstring(L,"");
    const int lib_files = lua_gettop(L);
    lua_pushstring(L,"");
    const int frameworks = lua_gettop(L);

    addall2(L,inst,ldflags,lib_dirs,lib_names,lib_files,frameworks,
            toolchain == BS_msvc || (win32 && toolchain == BS_clang), mac, win32);
            // clang on windows uses the lib.exe compatible llvm-lib.exe tool

    addPath(L,rootOutDir,relDir);
    lua_pushvalue(L,-1);
    lua_pushstring(L,"/");

    if( !win32 && ( kind == BS_DynamicLib || kind == BS_StaticLib ) )
        lua_pushstring(L,"lib"); // if not on Windows prefix the lib name with "lib"
    else
        lua_pushstring(L,"");
    lua_getfield(L,inst,"name");
    if( lua_isnil(L,-1) || lua_objlen(L,-1) == 0 )
    {
        lua_pop(L,1);
        lua_getfield(L,inst,"#decl");
        lua_getfield(L,-1,"#name");
        lua_replace(L,-2);
    }
    lua_concat(L,4);
    lua_replace(L,-2);
    const int outbase = lua_gettop(L);

    lua_pushvalue(L,outbase);
    switch(kind)
    {
    case BS_DynamicLib:
        if( win32 )
            lua_pushstring(L,".dll");
        else if(mac)
            lua_pushstring(L,".dylib");
        else
            lua_pushstring(L,".so");
        break;
    case BS_Executable:
        if( win32 )
            lua_pushstring(L,".exe");
        else
            lua_pushstring(L,"");
        break;
    case BS_StaticLib:
        if( win32 )
            lua_pushstring(L,".lib");
        else
            lua_pushstring(L,".a");
        break;
    }
    lua_concat(L,2);
    const int out = lua_gettop(L);

    const time_t outExists = bs_exists(lua_tostring(L,out));

    lua_pushvalue(L,outbase);
    lua_pushstring(L,".rsp");
    lua_concat(L,2);
    const int rsp = lua_gettop(L);

    int useRsp = 1;

    switch(toolchain)
    {
    case BS_gcc:
        // TODO: since we always use gcc (not g++ and the like) additional flags and libs might be needed
        // see https://stackoverflow.com/questions/172587/what-is-the-difference-between-g-and-gcc
        // g++ on link time is equivalent to gcc -lstdc++ -shared-libgcc
        switch(kind)
        {
        case BS_Executable:
            lua_pushfstring(L,"gcc @\"%s\" -o \"%s\"",
                            bs_denormalize_path(lua_tostring(L,rsp)),
                            bs_denormalize_path(lua_tostring(L,out)) );
            break;
        case BS_DynamicLib:
            lua_pushfstring(L,"gcc %s @\"%s\" -o \"%s\"",
                            (mac ? "-dynamiclib " : "-shared "),
                            bs_denormalize_path(lua_tostring(L,rsp)),
                            bs_denormalize_path(lua_tostring(L,out)) );
            break;
        case BS_StaticLib:
            if( !mac )
                lua_pushfstring(L,"ar r \"%s\" @\"%s\"",
                            bs_denormalize_path(lua_tostring(L,out)),
                            bs_denormalize_path(lua_tostring(L,rsp)) );
            else
            {
                useRsp = 0; // on macs only a few years old ar and gcc version 4 is installed which doesn't support @file
                lua_pushfstring(L,"ar r \"%s\" ", bs_denormalize_path(lua_tostring(L,out)) );
            }
            break;
        }
        break;
    case BS_clang:
        switch(kind)
        {
        case BS_Executable:
            lua_pushfstring(L,"clang @\"%s\" -o \"%s\"",
                            bs_denormalize_path(lua_tostring(L,rsp)),
                            bs_denormalize_path(lua_tostring(L,out)) );
            break;
        case BS_DynamicLib:
            lua_pushfstring(L,"clang %s @\"%s\" -o \"%s\"",
                            (mac ? "-dynamiclib " : "-shared "),
                            bs_denormalize_path(lua_tostring(L,rsp)),
                            bs_denormalize_path(lua_tostring(L,out)) );
            break;
        case BS_StaticLib:
            if( win32 )
                lua_pushfstring(L,"llvm-lib /nologo /out:\"%s\" @\"%s\"",
                            bs_denormalize_path(lua_tostring(L,out)),
                            bs_denormalize_path(lua_tostring(L,rsp)) );
            else
                lua_pushfstring(L,"ar r \"%s\" @\"%s\"",
                                bs_denormalize_path(lua_tostring(L,out)),
                                bs_denormalize_path(lua_tostring(L,rsp)) );
            break;
        }
        break;
    case BS_msvc:
        switch(kind)
        {
        case BS_Executable:
            lua_pushfstring(L,"link /nologo @\"%s\" /out:\"%s\"",
                            bs_denormalize_path(lua_tostring(L,rsp)),
                            bs_denormalize_path(lua_tostring(L,out)) );
            break;
        case BS_DynamicLib:
            // TODO: should we use -rpath=path ?
            lua_pushfstring(L,"link /nologo /dll @\"%s\" /out:\"%s\" /implib:\"%s.lib\"",
                            bs_denormalize_path(lua_tostring(L,rsp)),
                            bs_denormalize_path(lua_tostring(L,out)),
                            bs_denormalize_path(lua_tostring(L,out)) ); // the importlib is called xyz.dll.lib
            break;
        case BS_StaticLib:
            lua_pushfstring(L,"lib /nologo /out:\"%s\" @\"%s\"",
                            bs_denormalize_path(lua_tostring(L,out)),
                            bs_denormalize_path(lua_tostring(L,rsp)) );
            break;
        }
        break;
    }
    const int cmd = lua_gettop(L);

    lua_createtable(L,0,0);
    const int outlist = lua_gettop(L);
    lua_pushinteger(L,kind);
    lua_setfield(L,outlist,"#kind");
    lua_pushvalue(L,out);
    lua_rawseti(L,outlist,1);
    lua_pushvalue(L,outlist);
    lua_setfield(L,inst,"#out");
    lua_pop(L,1); // outlist

    time_t srcExists = 0;

    if( useRsp )
    {
        FILE* f = bs_fopen(bs_denormalize_path(lua_tostring(L,rsp)),"w");
        if( f == NULL )
            luaL_error(L, "cannot open rsp file for writing: %s", lua_tostring(L,rsp));

        srcExists = renderobjectfiles(L,inlist,f,0, toolchain == BS_msvc);

        fwrite(lua_tostring(L,ldflags),1,lua_objlen(L,ldflags),f);
        fwrite(lua_tostring(L,lib_dirs),1,lua_objlen(L,lib_dirs),f);
        fwrite(lua_tostring(L,lib_names),1,lua_objlen(L,lib_names),f);
        fwrite(lua_tostring(L,lib_files),1,lua_objlen(L,lib_files),f);
        fwrite(lua_tostring(L,frameworks),1,lua_objlen(L,frameworks),f);

        fclose(f);
    }else
    {
        // luaL_Buffer doesn't work; luaL_pushresult produces "attempt to concatenate a table value"
        srcExists = renderobjectfiles(L,inlist,0,cmd, toolchain == BS_msvc);
        // TODO lib_files
    }

    if( !outExists || outExists < srcExists )
    {
        fprintf(stdout,"%s\n", lua_tostring(L,cmd));
        fflush(stdout);
        if( bs_exec(lua_tostring(L,cmd)) != 0 ) // works for all gcc, clang and cl
        {
            // stderr was already written to the console
            lua_pushnil(L);
            lua_error(L);
        }
    }
    lua_pop(L,1); // cmd

    lua_pop(L,11); // binst, rootOutDir, relDir, ldflags...frameworks, outbase, out, rsp
    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static void builddeps(lua_State* L, int inst)
{
    const int top = lua_gettop(L);

    lua_getfield(L,inst,"deps");
    const int deps = lua_gettop(L);
    if( lua_isnil(L,deps) )
        return;

    lua_createtable(L,0,0);
    lua_pushinteger(L,BS_Mixed);
    lua_setfield(L,-2,"#kind");
    const int out = lua_gettop(L);

    const int ndeps = lua_objlen(L,deps);
    int nout = 0;
    int i;
    for( i = 1; i <= ndeps; i++ )
    {
        // TODO: check for circular deps
        lua_pushcfunction(L, bs_run);
        lua_rawgeti(L,deps,i);
        lua_call(L,1,1);
        // stack: dep_inst

        lua_getfield(L,-1,"#out");
        lua_replace(L,-2);
        // stack: subout

        const int subout = lua_gettop(L);
        int k = BS_Nothing;
        if( lua_istable(L,subout) )
        {
            lua_getfield(L,-1,"#kind");
            k = lua_tointeger(L,-1);
            lua_pop(L,1);
        }

        if( k == BS_Mixed )
        {
            const int nsubout = lua_objlen(L,subout);
            int j;
            for( j = 1; j <= nsubout; j++ )
            {
                lua_rawgeti(L,subout,j);

                lua_getfield(L,-1,"#kind");
                const int kk = lua_tointeger(L,-1);
                lua_pop(L,1);
                assert( kk != BS_Mixed);

                lua_rawseti(L,out,++nout);
            }
            lua_pop(L,1); // subout
        }else if( lua_istable(L,subout) )
            lua_rawseti(L,out,++nout); // eats subout
        else
            lua_pop(L,1);
    }
    lua_setfield(L,inst,"#out");
    lua_pop(L,1); // deps
    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static int makeCopyOfLibs(lua_State* L, int inlist)
{
    const int top = lua_gettop(L);
    lua_getfield(L,inlist,"#kind");
    const int k = lua_tointeger(L,-1);
    lua_pop(L,1); // kind

    assert( k == BS_Mixed );

    size_t i, len = 0;
    len = lua_objlen(L,inlist);
    int hasLibs = 0;
    for( i = 1; i <= len; i++ )
    {
        lua_rawgeti(L,inlist,i);
        const int sublist = lua_gettop(L);
        lua_getfield(L,sublist,"#kind");
        const int k = lua_tointeger(L,-1);
        lua_pop(L,2); // kind, sublist
        assert( k != BS_Mixed );
        if( k == BS_StaticLib || k == BS_DynamicLib )
        {
            hasLibs = 1;
            break;
        }
    }
    if( hasLibs )
    {
        lua_createtable(L,0,0);
        lua_pushinteger(L,BS_Mixed);
        lua_setfield(L,-2,"#kind");
        const int outlist = lua_gettop(L);
        int n = 0;
        for( i = 1; i <= len; i++ )
        {
            lua_rawgeti(L,inlist,i);
            const int sublist = lua_gettop(L);
            lua_getfield(L,sublist,"#kind");
            const int k = lua_tointeger(L,-1);
            lua_pop(L,1); // kind
            if( k == BS_StaticLib || k == BS_DynamicLib )
                lua_rawseti(L,outlist,++n);
            else
                lua_pop(L,1); // sublist
        }
    }
    assert( top + ( hasLibs ? 1: 0 ) == lua_gettop(L) );
    return hasLibs;
}

static void library(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);
    lua_getfield(L,inst,"#out");
    const int inlist = lua_gettop(L); // inlist is of kind BS_Mixed and doesn't have items of kind BS_Mixed
    assert( lua_istable(L,inlist) );
    compilesources(L,inst,builtins, inlist);

    lua_getfield(L,inst,"#out");
    if( makeCopyOfLibs(L,inlist) )
    {
        // inlist included libs, so top of stack is new BS_Mixed which includes these libs (and only those)
        lua_replace(L,inlist);
        lua_rawseti(L,inlist,lua_objlen(L,inlist)+1);
        // now the new BS_Mixed also includes the BS_ObjectFiles from compile output
    }else
        lua_replace(L,inlist); // make BS_ObjectFiles from compile output the new inlist

    lua_getfield(L,inst,"lib_type");
    // link sets out to a new table of kind BS_DynamicLib or BS_StaticLib; inlist is not passed out
    if( strcmp(lua_tostring(L,-1),"shared") == 0 )
        link(L,inst,builtins,inlist,BS_DynamicLib);
    else
        link(L,inst,builtins,inlist,BS_StaticLib);

    lua_pop(L,2); // out, lib_type
    assert( top == lua_gettop(L) );

    // passes on one lib (either BS_DynamicLib or BS_StaticLib)
}

static void executable(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);
    lua_getfield(L,inst,"#out");
    const int inlist = lua_gettop(L);
    assert( lua_istable(L,inlist) );
    compilesources(L,inst,builtins,inlist);

    lua_getfield(L,inst,"#out");
    if( makeCopyOfLibs(L,inlist) )
    {
        // inlist included libs, so top of stack is new BS_Mixed which includes these libs (and only those)
        lua_replace(L,inlist);
        lua_rawseti(L,inlist,lua_objlen(L,inlist)+1);
        // now the new BS_Mixed also includes the BS_ObjectFiles from compile output
    }else
        lua_replace(L,inlist); // make BS_ObjectFiles from compile output the new inlist

    link(L,inst,builtins,inlist,BS_Executable);
    lua_pop(L,1); // mixed
    assert( top == lua_gettop(L) );

    // passes on one BS_Executable
}

static void sourceset(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);
    lua_getfield(L,inst,"#out");
    const int inlist = lua_gettop(L);
    assert( lua_istable(L,inlist) );
    compilesources(L,inst,builtins,inlist);
    // #out is now a BS_ObjectFiles

    if( makeCopyOfLibs(L,inlist) )
    {
        // inlist included libs, so top of stack is new BS_Mixed which includes these libs (and only those)
        lua_replace(L,inlist);
        lua_getfield(L,inst,"#out");
        lua_rawseti(L,inlist,lua_objlen(L,inlist)+1); // add BS_ObjectFiles to the new BS_Mixed
        lua_setfield(L,inst,"#out"); // set mixed again as inst.#out
    }else
        lua_pop(L,1); // inlist

    // passes on a BS_ObjectFiles or a BS_Mixed with BS_ObjectFiles and the libs from orig inlist

    assert( top == lua_gettop(L) );
}

static void group(lua_State* L,int inst, int cls, int builtins)
{
    // NOP. deps were already built and result handed to inst.#out
}

static void config(lua_State* L,int inst, int cls, int builtins)
{
    // NOP
}

int bs_thisapp2(lua_State *L)
{
    // see https://stackoverflow.com/questions/933850/how-do-i-find-the-location-of-the-executable-in-c
    const int res = bs_thisapp();
    if( res == BS_NOP )
    {
        lua_getglobal(L,"#prog");
        const char* path = lua_tostring(L,-1);
        if( path[0] == '/' || path[0] == '\\' )
            return 1;
        if( strchr(path,'/') != 0 || strchr(path,'\\') != 0 )
        {
            if( bs_cwd() == BS_OK )
                lua_pushstring(L, bs_global_buffer());
            else
                luaL_error(L,"getcwd: received non supported path from OS");
            addPath(L,-1,-2);
            // stack: #prog, cwd, abspath
            lua_replace(L,-3);
            lua_pop(L,1);
            return 1;
        }else
            luaL_error(L,"thisapp: cannot determine path of current application");
    }else if( res == BS_OK )
        lua_pushstring(L, bs_global_buffer());
    else
        luaL_error(L,"thisapp: received non supported path from OS");
    return 1;
}

static void script(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);

#if 0
    lua_createtable(L,0,0);
    lua_pushinteger(L,BS_Nothing);
    lua_setfield(L,-2,"#kind");
#else
    lua_pushnil(L);
#endif
    lua_setfield(L,inst,"#out");

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);

    lua_getfield(L,inst,"script");
    const int script = lua_gettop(L);
    if( *lua_tostring(L,script) != '/' )
    {
        // relative path
        addPath(L,absDir,script);
        lua_replace(L,script);
    }

    bs_thisapp2(L);
    const int app = lua_gettop(L);

    lua_pushstring(L,"");
    const int args = lua_gettop(L);

    lua_getfield(L,inst,"args");
    addflags(L,-1,args);
    lua_pop(L,1);

    lua_pushfstring(L, "%s %s %s", bs_denormalize_path(lua_tostring(L,app) ),
                    bs_denormalize_path(lua_tostring(L,script) ),
                    lua_tostring(L,args) );
    const int cmd = lua_gettop(L);

    fprintf(stdout,"%s\n", lua_tostring(L,cmd));
    fflush(stdout);
    if( bs_exec(lua_tostring(L,cmd)) != 0 )
    {
        // stderr was already written to the console
        lua_pushnil(L);
        lua_error(L);
    }

    lua_pop(L,5); // abDir, script, app, args, cmd
    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static void runforeach(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);

#if 0
    lua_createtable(L,0,0);
    lua_pushinteger(L,BS_Nothing);
    lua_setfield(L,-2,"#kind");
#else
    lua_pushnil(L);
#endif
    lua_setfield(L,inst,"#out");

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);

    lua_getfield(L,inst,"script");
    const int script = lua_gettop(L);
    if( *lua_tostring(L,script) != '/' )
    {
        // relative path
        addPath(L,absDir,script);
        lua_replace(L,script);
    }

    bs_thisapp2(L);
    const int app = lua_gettop(L);

    size_t i;
    lua_getfield(L,inst,"sources");
    const int sources = lua_gettop(L);
    for( i = 1; i <= lua_objlen(L,sources); i++ )
    {
        lua_rawgeti(L,sources,i);
        const int source = lua_gettop(L);
        if( *lua_tostring(L,source) != '/' )
        {
            addPath(L,absDir,source);
            lua_replace(L,source);
        }

        lua_pushstring(L,"");
        const int args = lua_gettop(L);

        lua_getfield(L,inst,"args");
        const int arglist = lua_gettop(L);
        size_t j;
        for( j = 1; j <= lua_objlen(L,arglist); j++ )
        {
            lua_pushvalue(L,args);
            lua_pushstring(L," ");
            lua_rawgeti(L,arglist,j);
            if( bs_apply_source_expansion(lua_tostring(L,source),lua_tostring(L,-1), 0) != BS_OK )
                luaL_error(L,"cannot do source expansion, invalid placeholders in string: %s", lua_tostring(L,-1));
            lua_pop(L,1);
            lua_pushstring(L,bs_global_buffer());
            lua_concat(L,3);
            lua_replace(L,args);
        }
        lua_pop(L,1); // arglist

        lua_pushfstring(L, "%s %s %s", bs_denormalize_path(lua_tostring(L,app) ),
                        bs_denormalize_path(lua_tostring(L,script) ),
                        lua_tostring(L,args) );
        const int cmd = lua_gettop(L);

        fprintf(stdout,"%s\n", lua_tostring(L,cmd));
        fflush(stdout);
        if( bs_exec(lua_tostring(L,cmd)) != 0 )
        {
            // stderr was already written to the console
            lua_pushnil(L);
            lua_error(L);
        }
        lua_pop(L,3); // args, cmd, source
    }

    lua_pop(L,4); // abDir, script, app, sources
    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static void runmoc(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);

    lua_createtable(L,0,0);
    const int outlist = lua_gettop(L);
    lua_pushinteger(L,BS_SourceFiles);
    lua_setfield(L,outlist,"#kind");
    lua_pushvalue(L,outlist);
    lua_setfield(L,inst,"#out");

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);

    lua_getfield(L,builtins,"#inst");
    const int binst = lua_gettop(L);

    lua_getfield(L,binst,"root_build_dir");
    getModuleVarFrom(L,inst,"#rdir");
    addPath(L,-2,-1);
    lua_replace(L,-3);
    lua_pop(L,1);
    const int outDir = lua_gettop(L);

    lua_getfield(L,binst,"moc_path");
    const int app = lua_gettop(L);
    if( lua_isnil(L,app) || strcmp(".",lua_tostring(L,app)) == 0 )
    {
        lua_pushstring(L,"moc");
        lua_replace(L,app);
    }else if( *lua_tostring(L,app) != '/' )
    {
        // relative path
        addPath(L,absDir,app);
        lua_replace(L,app);
    }

    lua_pushstring(L,"");
    const int defines = lua_gettop(L);
    lua_getfield(L,inst,"defines");
    const int defs = lua_gettop(L);
    size_t i;
    for( i = 1; i <= lua_objlen(L,defs); i++ )
    {
        lua_rawgeti(L,defs,i);
        lua_pushvalue(L,defines);
        if( strstr(lua_tostring(L,-2),"\\\"") != NULL )
            lua_pushfstring(L," \"-D %s\"", lua_tostring(L,-2)); // strings can potentially include whitespace, thus quotes
        else
            lua_pushfstring(L," -D %s", lua_tostring(L,-2));
        lua_concat(L,2);
        lua_replace(L,defines);
        lua_pop(L,1); // def
    }
    lua_pop(L,1); // defs

    lua_getfield(L,inst,"sources");
    const int sources = lua_gettop(L);
    int n = 0;
    for( i = 1; i <= lua_objlen(L,sources); i++ )
    {
        lua_rawgeti(L,sources,i);
        const int source = lua_gettop(L);
        const int lang = guessLang(lua_tostring(L,source));

        if( *lua_tostring(L,source) != '/' )
        {
            addPath(L,absDir,source);
            lua_replace(L,source);
        }

        if( lang == BS_header )
            // this file is automatically passed to the compiler over the deps chain; the user doesn't see it
            lua_pushfstring(L,"%s/moc_%s.cpp",lua_tostring(L,outDir), bs_filename(lua_tostring(L,source)));
        else
        {
            int len;
            const char* name = bs_path_part(lua_tostring(L,source),BS_baseName,&len);
            lua_pushlstring(L,name,len);
            // this file has to be included at the bottom of the cpp file, so use the naming of the Qt documentation.
            lua_pushfstring(L,"%s/%s.moc",lua_tostring(L,outDir), bs_filename(lua_tostring(L,-1)));
            lua_replace(L,-2);
        }
        const int outFile = lua_gettop(L);

        if( lang == BS_header )
        {
            lua_pushvalue(L,outFile);
            lua_rawseti(L,outlist,++n);
        }

        // check if there is a {{source_dir}}/{{source_name_part}}_p.h and - if true - include it in the generated file
        lua_pushstring(L,"{{source_dir}}/{{source_name_part}}_p.h");
        bs_apply_source_expansion(lua_tostring(L,source),lua_tostring(L,-1), 0);
        const int includePrivateHeader = bs_exists2(bs_global_buffer());
        lua_pop(L,1);

        lua_pushfstring(L, "%s %s -o %s%s", bs_denormalize_path(lua_tostring(L,app) ),
                        bs_denormalize_path(lua_tostring(L,source) ),
                        bs_denormalize_path(lua_tostring(L,outFile)),
                        lua_tostring(L,defines));
        if( includePrivateHeader && lang == BS_header )
        {
            lua_pushstring(L," -p {{source_dir}}");
            bs_apply_source_expansion(lua_tostring(L,source),lua_tostring(L,-1), 0);
            lua_pushstring(L, bs_global_buffer());
            lua_replace(L,-2);
            lua_pushstring(L," -b {{source_name_part}}_p.h");
            bs_apply_source_expansion(lua_tostring(L,source),lua_tostring(L,-1), 0);
            lua_pushstring(L, bs_global_buffer());
            lua_replace(L,-2);
            lua_concat(L,3);
        }
        const int cmd = lua_gettop(L);

        const time_t srcExists = bs_exists(lua_tostring(L,source));
        const time_t outExists = bs_exists(lua_tostring(L,outFile));

        if( !outExists || outExists < srcExists )
        {
            fprintf(stdout,"%s\n", lua_tostring(L,cmd));
            fflush(stdout);
            // only call if outfile is older than source
            if( bs_exec(lua_tostring(L,cmd)) != 0 )
            {
                // stderr was already written to the console
                lua_pushnil(L);
                lua_error(L);
            }
        }
        lua_pop(L,3); // cmd, source, outFile
    }

    lua_pop(L,7); // outlist absDir binst outDir defines app sources
    const int bottom = lua_gettop(L);
    assert( top == bottom );

    // passes on one BS_SourceFiles
}

static void runrcc(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);

    lua_createtable(L,0,0);
    const int outlist = lua_gettop(L);
    lua_pushinteger(L,BS_SourceFiles);
    lua_setfield(L,outlist,"#kind");
    lua_pushvalue(L,outlist);
    lua_setfield(L,inst,"#out");

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);

    lua_getfield(L,builtins,"#inst");
    const int binst = lua_gettop(L);

    lua_getfield(L,binst,"root_build_dir");
    getModuleVarFrom(L,inst,"#rdir");
    addPath(L,-2,-1);
    lua_replace(L,-3);
    lua_pop(L,1);
    const int outDir = lua_gettop(L);

    lua_getfield(L,binst,"rcc_path");
    const int app = lua_gettop(L);
    if( lua_isnil(L,app) || strcmp(".",lua_tostring(L,app)) == 0 )
    {
        lua_pushstring(L,"rcc");
        lua_replace(L,app);
    }else if( *lua_tostring(L,app) != '/' )
    {
        // relative path
        addPath(L,absDir,app);
        lua_replace(L,app);
    }

    size_t i;
    lua_getfield(L,inst,"sources");
    const int sources = lua_gettop(L);
    for( i = 1; i <= lua_objlen(L,sources); i++ )
    {
        lua_rawgeti(L,sources,i);
        const int source = lua_gettop(L);
        if( *lua_tostring(L,source) != '/' )
        {
            addPath(L,absDir,source);
            lua_replace(L,source);
        }

        lua_pushfstring(L,"%s/qrc_%s.cpp",lua_tostring(L,outDir), bs_filename(lua_tostring(L,source)));
        const int outFile = lua_gettop(L);

        lua_pushvalue(L,outFile);
        lua_rawseti(L,outlist,i);

        int len = 0;
        const char* name = bs_path_part(lua_tostring(L,source),BS_baseName, &len);
        lua_pushlstring(L,name,len);
        lua_pushfstring(L, "%s %s -o %s -name %s", bs_denormalize_path(lua_tostring(L,app) ),
                        bs_denormalize_path(lua_tostring(L,source) ),
                        bs_denormalize_path(lua_tostring(L,outFile)),
                        lua_tostring(L,-1));
        lua_replace(L,-2);
        const int cmd = lua_gettop(L);

        const time_t srcExists = bs_exists(lua_tostring(L,source));
        const time_t outExists = bs_exists(lua_tostring(L,outFile));

        if( !outExists || outExists < srcExists )
        {
            fprintf(stdout,"%s\n", lua_tostring(L,cmd));
            fflush(stdout);
            // only call if outfile is older than source
            if( bs_exec(lua_tostring(L,cmd)) != 0 )
            {
                // stderr was already written to the console
                lua_pushnil(L);
                lua_error(L);
            }
        }
        lua_pop(L,3); // cmd, source, outFile
    }

    lua_pop(L,6); // outlist absDir binst outDir app sources
    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static void copy(lua_State* L,int inst, int cls, int builtins)
{
    const int top = lua_gettop(L);

    lua_getfield(L,inst,"#out");
    const int inlist = lua_gettop(L); // inlist is of kind BS_Mixed and doesn't have items of kind BS_Mixed
    assert( lua_istable(L,inlist) );

    lua_pushnil(L);
    lua_setfield(L,inst,"#out");

    getModuleVarFrom(L,inst,"#dir");
    const int absDir = lua_gettop(L);

    lua_getfield(L,builtins,"#inst");
    lua_getfield(L,-1,"root_build_dir");
    lua_replace(L,-2);
    getModuleVarFrom(L,inst,"#rdir");
    addPath(L,-2,-1); // root_build_dir, rdir, root_build_dir+rdir
    lua_replace(L,-3);
    lua_pop(L,1);
    const int outDir = lua_gettop(L);

    size_t i;

    lua_getfield(L,inst,"sources");
    const int sources = lua_gettop(L);
    lua_createtable(L,lua_objlen(L,sources),0);
    for( i = 1; i <= lua_objlen(L,sources); i++ )
    {
        // copy all sources to a new table to avoid changing the original sources
        lua_rawgeti(L,sources,i);
        lua_rawseti(L,-2,i);
    }
    lua_replace(L,-2);

    lua_getfield(L,inst,"use_deps");
    const int use_deps = lua_gettop(L);
    assert(lua_istable(L,use_deps));
    for( i = 1; i <= lua_objlen(L,use_deps); i++ )
    {
        lua_rawgeti(L,use_deps,i);
        const char* what = lua_tostring(L,-1);
        if( strcmp(what,"object_file") == 0 )
            copyItems(L,inlist,sources, BS_ObjectFiles);
        else if( strcmp(what,"source_file") == 0 )
            copyItems(L,inlist,sources, BS_SourceFiles);
        else if( strcmp(what,"static_lib") == 0 )
            copyItems(L,inlist,sources, BS_StaticLib);
        else if( strcmp(what,"shared_lib") == 0 )
            copyItems(L,inlist,sources, BS_DynamicLib);
        else if( strcmp(what,"executable") == 0 )
            copyItems(L,inlist,sources, BS_Executable);
        else
            assert(0);
        lua_pop(L,1); // use_deps item
    }
    lua_pop(L,1); // use_deps

    lua_getfield(L,inst,"outputs");
    const int outputs = lua_gettop(L);
    const int len = lua_objlen(L,outputs);
    if( len == 0 )
    {
        lua_getfield(L,inst,"#decl");
        calcdesig(L,-1);
        luaL_error(L,"outputs in Copy instance '%s' cannot be empty", lua_tostring(L,-1));
    }

    for( i = 1; i <= lua_objlen(L,sources); i++ )
    {
        lua_rawgeti(L,sources,i);
        const int from = lua_gettop(L);
        if( *lua_tostring(L,from) != '/' )
        {
            addPath(L,absDir,from);
            lua_replace(L,from);
        }

        size_t j;
        for( j = 1; j <= len; j++ )
        {
            lua_rawgeti(L,outputs,j);
            const int to = lua_gettop(L);

            if( bs_apply_source_expansion(lua_tostring(L,from),lua_tostring(L,to), 1) != BS_OK )
                luaL_error(L,"cannot do source expansion, invalid placeholders in path: %s", lua_tostring(L,to));
            lua_pushstring(L,bs_global_buffer());
            lua_replace(L,to);
            if( *lua_tostring(L,to) != '/' )
            {
                addPath(L,outDir,to);
                lua_replace(L,to);
            }else
            {
                lua_getfield(L,inst,"#decl");
                calcdesig(L,-1);
                luaL_error(L,"outputs in Copy instance '%s' require relative paths", lua_tostring(L,-1));
            }

            if( bs_copy(lua_tostring(L,to), lua_tostring(L,from) ))
                luaL_error(L,"cannot copy %s to %s", lua_tostring(L,from), lua_tostring(L,to));

            lua_pop(L,1); // to
        }
        lua_pop(L,1); // from
    }

    lua_pop(L,5); // inlist, absDir, OutDir, sources, outputs
    const int bottom = lua_gettop(L);
    assert( top == bottom );
}

static void message(lua_State* L,int inst, int cls, int builtins)
{
    lua_getfield(L,inst,"msg_type");
    const int msg_type = lua_gettop(L);
    if( strcmp(lua_tostring(L,msg_type),"error") == 0 )
    {
        lua_pushstring(L,"# ERR: ");
        lua_getfield(L,inst,"text");
        lua_concat(L,2);
        lua_error(L);
    }else if( strcmp(lua_tostring(L,msg_type),"warning") == 0 )
    {
        lua_getfield(L,inst,"text");
        fprintf(stderr,"# WRN: %s\n", lua_tostring(L,-1));
        fflush(stderr);
    }else
    {
        lua_getfield(L,inst,"text");
        fprintf(stdout,"# %s\n", lua_tostring(L,lua_gettop(L)));
        fflush(stdout);
    }
    lua_pop(L,2); // msg_type, text
}

int bs_createBuildDirs(lua_State* L) // lua function; params: rootModuleDef, rootPath
{
    enum { rootModule = 1, rootPath = 2 };
    if( !bs_exists(lua_tostring(L,rootPath)) )
    {
        if( bs_mkdir(lua_tostring(L,rootPath)) != 0 )
            luaL_error(L,"error creating directory %s", lua_tostring(L,-1));
        fflush(stdout);
    }
    size_t i;
    for( i = 1; i <= lua_objlen(L,rootModule); i++ )
    {
        lua_pushcfunction(L, bs_createBuildDirs);

        lua_rawgeti(L,rootModule,i);
        const int module = lua_gettop(L);

        lua_getfield(L,module,"#kind");
        const int k = lua_tointeger(L,-1);
        lua_pop(L,1);

        if( k == BS_ModuleDef )
        {
            lua_pushvalue(L,rootPath);
            lua_pushstring(L,"/");
            lua_getfield(L,module,"#dirname");
            lua_concat(L,3);
            lua_call(L,2,0);
        }else
            lua_pop(L,2); // func, module
    }
    return 0;
}

int bs_run(lua_State* L) // args: productinst, returns: inst
{
    const int inst = 1;

    lua_getfield(L,inst,"#out");
    const int built = !lua_isnil(L,-1);
    lua_pop(L,1);
    if( built )
        return 1; // we're already built

    lua_getmetatable(L,inst);
    const int cls = lua_gettop(L);

    lua_getglobal(L, "require");
    lua_pushstring(L, "builtins");
    lua_call(L,1,1);
    const int builtins = lua_gettop(L);

    lua_getfield(L,cls,"#name");
    const char* name = lua_tostring(L,-1);

    builddeps(L,inst);

    lua_getfield(L,inst,"#decl");
    calcdesig(L,-1);
    fprintf(stdout,"# building %s %s\n",name,lua_tostring(L,-1));
    fflush(stdout);
    lua_pop(L,2); // decl, name

    // use isa instead of strcmp so that users can subclass the built-in classes
    if( isa( L, builtins, cls, "Library" ) )
        library(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "Executable") )
        executable(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "SourceSet") )
        sourceset(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "Group") )
        group(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "Config") )
        config(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "LuaScript") )
        script(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "LuaScriptForeach") )
        runforeach(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "Copy") )
        copy(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "Message") )
        message(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "Moc") )
        runmoc(L,inst,cls,builtins);
    else if( isa( L, builtins, cls, "Rcc") )
        runrcc(L,inst,cls,builtins);
    else
        luaL_error(L,"don't know how to build instances of class '%s'", name);

    lua_pop(L,3); // cls, builtins, name
    return 1; // inst
}
