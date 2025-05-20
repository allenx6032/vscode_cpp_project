/**********************************************************************************************
 *
 *   raylib-lua v2.0 - raylib Lua bindings for raylib v2.0
 *
 *   NOTES:
 *
 *   The following types are treated as objects with named fields, same as in C.
 *       Color, Vector2, Vector3, Rectangle, Ray, Camera, Camera2D
 *
 *   Lua defines utility functions to create those objects.
 *
 *   USAGE EXAMPLE:
 *       local cl = Color(255,255,255,255)
 *       local rec = Rectangle(10, 10, 100, 100)
 *       local ray = Ray(Vector3(20, 20, 20), Vector3(50, 50, 50))
 *       local x2 = rec.x + rec.width
 *
 *   The following types are immutable, and you can only read their non-pointer
 *arguments. Image, Texture2D, RenderTexture2D, Font
 *
 *   All other object types are opaque, that is, you cannot access or change
 *their fields directly.
 *
 *   Remember that ALL raylib types have REFERENCE SEMANTICS in Lua.
 *   There is currently no way to create a copy of an opaque object.
 *
 *   Some raylib functions take a pointer to an array, and the size of that
 *array. The equivalent Lua functions take only an array table of the specified
 *type UNLESS it's a pointer to a large char array (e.g. for images), then it
 *takes (and potentially returns) a Lua string (without the size argument, as
 *Lua strings are sized by default).
 *
 *   Some raylib functions take pointers to objects to modify (e.g.
 *ImageToPOT(), etc.) In Lua, these functions take values and return a new
 *changed value, instead.
 *
 *   So, in C:           ImageToPOT(&image, BLACK);
 *   In Lua becomes:     image = ImageToPOT(image, BLACK)
 *
 *   Remember that Lua functions can return multiple values.
 *   This is to preserve value semantics of raylib objects.
 *
 *   CONTRIBUTORS:
 *       Ghassan Al-Mashareqa (ghassan@ghassan.pl): Original binding creation
 *(for raylib 1.3) Ramon Santamaria (@raysan5): Review, update and maintenance
 *
 *
 *   LICENSE: zlib/libpng
 *
 *   Copyright (c) 2015-2017 Ghassan Al-Mashareqa and Ramon Santamaria
 *(@raysan5)
 *
 *   This software is provided "as-is", without any express or implied warranty.
 *In no event will the authors be held liable for any damages arising from the
 *use of this software.
 *
 *   Permission is granted to anyone to use this software for any purpose,
 *including commercial applications, and to alter it and redistribute it freely,
 *subject to the following restrictions:
 *
 *     1. The origin of this software must not be misrepresented; you must not
 *claim that you wrote the original software. If you use this software in a
 *product, an acknowledgment in the product documentation would be appreciated
 *but is not required.
 *
 *     2. Altered source versions must be plainly marked as such, and must not
 *be misrepresented as being the original software.
 *
 *     3. This notice may not be removed or altered from any source
 *distribution.
 *
 **********************************************************************************************/

#pragma once

#include "raylib.h"
#include "raymath.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#define LuaGetArgument_Image(L, img) *(Image *)LuaGetArgumentOpaqueTypeWithMetatable(L, img, "Image")
#define LuaGetArgument_Texture2D(L, tex) *(Texture2D *)LuaGetArgumentOpaqueTypeWithMetatable(L, tex, "Texture2D")
#define LuaGetArgument_RenderTexture2D(L, rtex)                                                                        \
    *(RenderTexture2D *)LuaGetArgumentOpaqueTypeWithMetatable(L, rtex, "RenderTexture2D")
#define LuaGetArgument_Font(L, sf) *(Font *)LuaGetArgumentOpaqueTypeWithMetatable(L, sf, "Font")
#define LuaGetArgument_Mesh(L, vd) *(Mesh *)LuaGetArgumentOpaqueType(L, vd)
#define LuaGetArgument_Shader(L, s) *(Shader *)LuaGetArgumentOpaqueType(L, s)
#define LuaGetArgument_Sound(L, snd) *(Sound *)LuaGetArgumentOpaqueType(L, snd)
#define LuaGetArgument_Wave(L, wav) *(Wave *)LuaGetArgumentOpaqueType(L, wav)
#define LuaGetArgument_Music(L, mus) *(Music *)LuaGetArgumentOpaqueType(L, mus)
#define LuaGetArgument_AudioStream(L, aud) *(AudioStream *)LuaGetArgumentOpaqueType(L, aud)
#define LuaGetArgument_PhysicsBody(L, body) *(PhysicsBody *)LuaGetArgumentOpaqueType(L, body)

#define LuaPushOpaqueType(L, str) LuaPushOpaque(L, &str, sizeof(str))
#define LuaPushOpaqueTypeWithMetatable(L, str, meta) LuaPushOpaqueWithMetatable(L, &str, sizeof(str), #meta)

#define LuaPush_int(L, value) lua_pushinteger(L, value)
#define LuaPush_float(L, value) lua_pushnumber(L, value);
#define LuaPush_bool(L, value) lua_pushboolean(L, value)
#define LuaPush_string(L, value) lua_pushstring(L, value)

#define LuaPush_Image(L, img) LuaPushOpaqueTypeWithMetatable(L, img, Image)
#define LuaPush_Texture2D(L, tex) LuaPushOpaqueTypeWithMetatable(L, tex, Texture2D)
#define LuaPush_RenderTexture2D(L, tex) LuaPushOpaqueTypeWithMetatable(L, tex, RenderTexture2D)
#define LuaPush_Font(L, sf) LuaPushOpaqueTypeWithMetatable(L, sf, Font)
#define LuaPush_Mesh(L, vd) LuaPushOpaqueType(L, vd)
#define LuaPush_Shader(L, s) LuaPushOpaqueType(L, s)
#define LuaPush_Sound(L, snd) LuaPushOpaqueType(L, snd)
#define LuaPush_Wave(L, wav) LuaPushOpaqueType(L, wav)
#define LuaPush_Music(L, mus) LuaPushOpaqueType(L, mus)
#define LuaPush_AudioStream(L, aud) LuaPushOpaqueType(L, aud)
#define LuaPush_PhysicsBody(L, body) LuaPushOpaqueType(L, body)

#define LuaGetArgument_ptr (void *)luaL_checkinteger
#define LuaGetArgument_int (int)luaL_checkinteger
#define LuaGetArgument_bool LuaGetArgument_int
#define LuaGetArgument_unsigned (unsigned)luaL_checkinteger
#define LuaGetArgument_char (char)luaL_checkinteger
#define LuaGetArgument_float (float)luaL_checknumber
#define LuaGetArgument_double luaL_checknumber
#define LuaGetArgument_string luaL_checkstring

    //----------------------------------------------------------------------------------
    // Global Variables Definition
    //----------------------------------------------------------------------------------

#ifdef RAYLIB_LUA_IMPLEMENTATION
    lua_State *mainLuaState = 0;
    lua_State *L = 0;
#else
extern lua_State *mainLuaState;
extern lua_State *L;
#endif

    //----------------------------------------------------------------------------------
    // Forward Declarations
    //----------------------------------------------------------------------------------
    inline static void LuaPush_Color(lua_State *L, Color color);

    //----------------------------------------------------------------------------------
    // rlua Helper Functions
    //----------------------------------------------------------------------------------
    inline static void LuaStartEnum(void)
    {
        lua_newtable(L);
    }

    inline static void LuaSetEnum(const char *name, int value)
    {
        LuaPush_int(L, value);
        lua_setfield(L, -2, name);
    }

    inline static void LuaSetEnumColor(const char *name, Color color)
    {
        LuaPush_Color(L, color);
        lua_setfield(L, -2, name);
    }

    inline static void LuaEndEnum(const char *name)
    {
        lua_setglobal(L, name);
    }

    inline static void LuaPushOpaque(lua_State *L, void *ptr, size_t size)
    {
        void *ud = lua_newuserdata(L, size);
        memcpy(ud, ptr, size);
    }

    inline static void LuaPushOpaqueWithMetatable(lua_State *L, void *ptr, size_t size, const char *metatable_name)
    {
        void *ud = lua_newuserdata(L, size);
        memcpy(ud, ptr, size);
        luaL_setmetatable(L, metatable_name);
    }

    inline static void *LuaGetArgumentOpaqueType(lua_State *L, int index)
    {
        return lua_touserdata(L, index);
    }

    inline static void *LuaGetArgumentOpaqueTypeWithMetatable(lua_State *L, int index, const char *metatable_name)
    {
        return luaL_checkudata(L, index, metatable_name);
    }

    //----------------------------------------------------------------------------------
    // LuaIndex* functions
    //----------------------------------------------------------------------------------
    static int LuaIndexImage(lua_State *L)
    {
        Image img = LuaGetArgument_Image(L, 1);
        const char *key = luaL_checkstring(L, 2);

        if (!strcmp(key, "width"))
            LuaPush_int(L, img.width);
        else if (!strcmp(key, "height"))
            LuaPush_int(L, img.height);
        else if (!strcmp(key, "mipmaps"))
            LuaPush_int(L, img.mipmaps);
        else if (!strcmp(key, "format"))
            LuaPush_int(L, img.format);
        else
            return 0;

        return 1;
    }

    static int LuaIndexTexture2D(lua_State *L)
    {
        Texture2D img = LuaGetArgument_Texture2D(L, 1);
        const char *key = luaL_checkstring(L, 2);

        if (!strcmp(key, "width"))
            LuaPush_int(L, img.width);
        else if (!strcmp(key, "height"))
            LuaPush_int(L, img.height);
        else if (!strcmp(key, "mipmaps"))
            LuaPush_int(L, img.mipmaps);
        else if (!strcmp(key, "format"))
            LuaPush_int(L, img.format);
        else if (!strcmp(key, "id"))
            LuaPush_int(L, img.id);
        else
            return 0;

        return 1;
    }

    static int LuaIndexRenderTexture2D(lua_State *L)
    {
        RenderTexture2D img = LuaGetArgument_RenderTexture2D(L, 1);
        const char *key = luaL_checkstring(L, 2);

        if (!strcmp(key, "texture"))
            LuaPush_Texture2D(L, img.texture);
        else if (!strcmp(key, "depth"))
            LuaPush_Texture2D(L, img.depth);
        else
            return 0;

        return 1;
    }

    static int LuaIndexFont(lua_State *L)
    {
        Font img = LuaGetArgument_Font(L, 1);
        const char *key = luaL_checkstring(L, 2);

        if (!strcmp(key, "baseSize"))
            LuaPush_int(L, img.baseSize);
        else if (!strcmp(key, "glyphCount"))
            LuaPush_int(L, img.glyphCount);
        else if (!strcmp(key, "glyphPadding"))
            LuaPush_int(L, img.glyphPadding);
        else if (!strcmp(key, "texture"))
            LuaPush_Texture2D(L, img.texture);
        else
            return 0;

        return 1;
    }

    static void LuaBuildOpaqueMetatables(void)
    {
        luaL_newmetatable(L, "Image");
        lua_pushcfunction(L, &LuaIndexImage);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);

        luaL_newmetatable(L, "Texture2D");
        lua_pushcfunction(L, &LuaIndexTexture2D);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);

        luaL_newmetatable(L, "RenderTexture2D");
        lua_pushcfunction(L, &LuaIndexRenderTexture2D);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);

        luaL_newmetatable(L, "Font");
        lua_pushcfunction(L, &LuaIndexFont);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);
    }

    //----------------------------------------------------------------------------------
    // LuaGetArgument functions
    //----------------------------------------------------------------------------------

    // Vector2 type
    inline static Vector2 LuaGetArgument_Vector2(lua_State *L, int index)
    {
        Vector2 result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "x") == LUA_TNUMBER, index, "Expected Vector2.x");
        result.x = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "y") == LUA_TNUMBER, index, "Expected Vector2.y");
        result.y = LuaGetArgument_float(L, -1);
        lua_pop(L, 2);
        return result;
    }

    // Vector3 type
    inline static Vector3 LuaGetArgument_Vector3(lua_State *L, int index)
    {
        Vector3 result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "x") == LUA_TNUMBER, index, "Expected Vector3.x");
        result.x = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "y") == LUA_TNUMBER, index, "Expected Vector3.y");
        result.y = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "z") == LUA_TNUMBER, index, "Expected Vector3.z");
        result.z = LuaGetArgument_float(L, -1);
        lua_pop(L, 3);
        return result;
    }

    // Vector4 type
    inline static Vector4 LuaGetArgument_Vector4(lua_State *L, int index)
    {
        Vector4 result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "x") == LUA_TNUMBER, index, "Expected Vector4.x");
        result.x = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "y") == LUA_TNUMBER, index, "Expected Vector4.y");
        result.y = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "z") == LUA_TNUMBER, index, "Expected Vector4.z");
        result.z = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "w") == LUA_TNUMBER, index, "Expected Vector4.w");
        result.w = LuaGetArgument_float(L, -1);
        lua_pop(L, 4);
        return result;
    }

    inline static Quaternion LuaGetArgument_Quaternion(lua_State *L, int index)
    {
        Quaternion result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "x") == LUA_TNUMBER, index, "Expected Quaternion.x");
        result.x = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "y") == LUA_TNUMBER, index, "Expected Quaternion.y");
        result.y = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "z") == LUA_TNUMBER, index, "Expected Quaternion.z");
        result.z = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "w") == LUA_TNUMBER, index, "Expected Quaternion.w");
        result.w = LuaGetArgument_float(L, -1);
        lua_pop(L, 4);
        return result;
    }

    // Matrix type
    inline static Matrix LuaGetArgument_Matrix(lua_State *L, int index)
    {
        Matrix result = {0};
        float *ptr = &result.m0;
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values

        for (int i = 0; i < 16; i++)
        {
            lua_geti(L, index, i + 1);
            ptr[i] = luaL_checknumber(L, -1);
        }
        lua_pop(L, 16);
        return result;
    }

    // Color type, RGBA (32bit)
    inline static Color LuaGetArgument_Color(lua_State *L, int index)
    {
        Color result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "r") == LUA_TNUMBER, index, "Expected Color.r");
        result.r = LuaGetArgument_unsigned(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "g") == LUA_TNUMBER, index, "Expected Color.g");
        result.g = LuaGetArgument_unsigned(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "b") == LUA_TNUMBER, index, "Expected Color.b");
        result.b = LuaGetArgument_unsigned(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "a") == LUA_TNUMBER, index, "Expected Color.a");
        result.a = LuaGetArgument_unsigned(L, -1);
        lua_pop(L, 4);
        return result;
    }

    // Rectangle type
    inline static Rectangle LuaGetArgument_Rectangle(lua_State *L, int index)
    {
        Rectangle result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "x") == LUA_TNUMBER, index, "Expected Rectangle.x");
        result.x = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "y") == LUA_TNUMBER, index, "Expected Rectangle.y");
        result.y = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "width") == LUA_TNUMBER, index, "Expected Rectangle.width");
        result.width = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "height") == LUA_TNUMBER, index, "Expected Rectangle.height");
        result.height = LuaGetArgument_float(L, -1);
        lua_pop(L, 4);
        return result;
    }

    // Image type -> Opaque

    // Texture2D type -> Opaque

    // RenderTexture2D type -> Opaque

    // Font character info type
    inline static GlyphInfo LuaGetArgument_CharInfo(lua_State *L, int index)
    {
        GlyphInfo result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "value") == LUA_TNUMBER, index, "Expected GlyphInfo.value");
        result.value = LuaGetArgument_int(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "offsetX") == LUA_TNUMBER, index, "Expected GlyphInfo.offsetX");
        result.offsetX = LuaGetArgument_int(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "offsetY") == LUA_TNUMBER, index, "Expected GlyphInfo.offsetY");
        result.offsetY = LuaGetArgument_int(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "advanceX") == LUA_TNUMBER, index, "Expected GlyphInfo.advanceX");
        result.advanceX = LuaGetArgument_int(L, -1);
        // luaL_argcheck(L, lua_getfield(L, index, "image") == LUA_TNUMBER, index,
        // "Expected GlyphInfo.advanceX");
        result.image = LuaGetArgument_Image(L, -1);
        lua_pop(L, 5);
        return result;
    }

    // Font type -> Opaque

    // Camera type, defines a camera position/orientation in 3d space
    inline static Camera LuaGetArgument_Camera(lua_State *L, int index)
    {
        Camera result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "position") == LUA_TTABLE, index, "Expected Camera.position");
        result.position = LuaGetArgument_Vector3(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "target") == LUA_TTABLE, index, "Expected Camera.target");
        result.target = LuaGetArgument_Vector3(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "up") == LUA_TTABLE, index, "Expected Camera.up");
        result.up = LuaGetArgument_Vector3(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "fovy") == LUA_TNUMBER, index, "Expected Camera.fovy");
        result.fovy = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "projection") == LUA_TNUMBER, index, "Expected Camera.projection");
        result.projection = LuaGetArgument_int(L, -1);
        lua_pop(L, 5);
        return result;
    }

    // Camera2D type, defines a 2d camera
    inline static Camera2D LuaGetArgument_Camera2D(lua_State *L, int index)
    {
        Camera2D result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "offset") == LUA_TTABLE, index, "Expected Camera2D.offset");
        result.offset = LuaGetArgument_Vector2(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "target") == LUA_TTABLE, index, "Expected Camera2D.target");
        result.target = LuaGetArgument_Vector2(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "rotation") == LUA_TNUMBER, index, "Expected Camera2D.rotation");
        result.rotation = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "zoom") == LUA_TNUMBER, index, "Expected Camera2D.zoom");
        result.zoom = LuaGetArgument_float(L, -1);
        lua_pop(L, 4);
        return result;
    }

    // Bounding box type
    inline static BoundingBox LuaGetArgument_BoundingBox(lua_State *L, int index)
    {
        BoundingBox result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "min") == LUA_TNUMBER, index, "Expected BoundingBox.min");
        result.min = LuaGetArgument_Vector3(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "max") == LUA_TNUMBER, index, "Expected BoundingBox.max");
        result.max = LuaGetArgument_Vector3(L, -1);
        lua_pop(L, 2);
        return result;
    }

    // Mesh type -> Opaque

    // Shader type -> Opaque

    // Material texture map
    inline static MaterialMap LuaGetArgument_MaterialMap(lua_State *L, int index)
    {
        MaterialMap result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "texture") == LUA_TNUMBER, index, "Expected MaterialMap.texture");
        result.texture = LuaGetArgument_Texture2D(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "color") == LUA_TNUMBER, index, "Expected MaterialMap.color");
        result.color = LuaGetArgument_Color(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "value") == LUA_TNUMBER, index, "Expected MaterialMap.value");
        result.value = LuaGetArgument_float(L, -1);
        lua_pop(L, 3);
        return result;
    }

    // Material type -> REVIEW
    /*
    static Material LuaGetArgument_Material(lua_State *L, int index)
    {
        Material result = { 0 };
        index = lua_absindex(L, index); // Makes sure we use absolute indices
    because we push multiple values luaL_argcheck(L, lua_getfield(L, index,
    "shader") == LUA_TNUMBER, index, "Expected Material.shader"); result.shader =
    LuaGetArgument_Shader(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "maps[MAX_MATERIAL_MAPS]") == LUA_TNUMBER, index, "Expected
    Material.maps[MAX_MATERIAL_MAPS]"); result.maps[MAX_MATERIAL_MAPS] =
    LuaGetArgument_MaterialMap(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "*params") == LUA_TNUMBER, index, "Expected Material.*params"); result.*params =
    LuaGetArgument_float(L, -1); lua_pop(L, 3); return result;
    }

    static Material LuaGetArgument_Material(lua_State* L, int index)
    {
        Material result;
        index = lua_absindex(L, index); // Makes sure we use absolute indices
    because we push multiple values luaL_argcheck(L, lua_getfield(L, index,
    "shader") == LUA_TUSERDATA, index, "Expected Material"); result.shader =
    LuaGetArgument_Shader(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "texDiffuse") == LUA_TUSERDATA, index, "Expected Material"); result.texDiffuse =
    LuaGetArgument_Texture2D(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "texNormal") == LUA_TUSERDATA, index, "Expected Material"); result.texNormal =
    LuaGetArgument_Texture2D(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "texSpecular") == LUA_TUSERDATA, index, "Expected Material"); result.texSpecular
    = LuaGetArgument_Texture2D(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "colDiffuse") == LUA_TTABLE, index, "Expected Material"); result.colDiffuse =
    LuaGetArgument_Color(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "colAmbient") == LUA_TTABLE, index, "Expected Material"); result.colAmbient =
    LuaGetArgument_Color(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "colSpecular") == LUA_TTABLE, index, "Expected Material"); result.colSpecular =
    LuaGetArgument_Color(L, -1); luaL_argcheck(L, lua_getfield(L, index,
    "glossiness") == LUA_TNUMBER, index, "Expected Material"); result.glossiness =
    LuaGetArgument_float(L, -1); lua_pop(L, 8); return result;
    }
    */

    // Model type
    inline static Model LuaGetArgument_Model(lua_State *L, int index)
    {
        assert(0 && "Unsupported");
        /*
        Model result = { 0 };
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
        we push multiple values luaL_argcheck(L, lua_getfield(L, index, "mesh") ==
        LUA_TNUMBER, index, "Expected Model.mesh"); result.mesh =
        LuaGetArgument_Mesh(L, -1); luaL_argcheck(L, lua_getfield(L, index,
        "transform") == LUA_TNUMBER, index, "Expected Model.transform");
        result.transform = LuaGetArgument_Matrix(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "material") == LUA_TNUMBER, index,
        "Expected Model.material"); result.material = LuaGetArgument_Material(L, -1);
        lua_pop(L, 3);
         */
        return (Model){0};
    }

    // Ray type (useful for raycast)
    inline static Ray LuaGetArgument_Ray(lua_State *L, int index)
    {
        Ray result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "position") == LUA_TNUMBER, index, "Expected Ray.position");
        result.position = LuaGetArgument_Vector3(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "direction") == LUA_TNUMBER, index, "Expected Ray.direction");
        result.direction = LuaGetArgument_Vector3(L, -1);
        lua_pop(L, 2);
        return result;
    }

    // Raycast hit information
    inline static RayCollision LuaGetArgument_RayHitInfo(lua_State *L, int index)
    {
        RayCollision result = {0};
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
                                        // we push multiple values
        luaL_argcheck(L, lua_getfield(L, index, "hit") == LUA_TNUMBER, index, "Expected RayCollision.hit");
        result.hit = LuaGetArgument_bool(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "distance") == LUA_TNUMBER, index, "Expected RayCollision.distance");
        result.distance = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "point") == LUA_TNUMBER, index, "Expected RayCollision.point");
        result.point = LuaGetArgument_Vector3(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "normal") == LUA_TNUMBER, index, "Expected RayCollision.normal");
        result.normal = LuaGetArgument_Vector3(L, -1);
        lua_pop(L, 4);
        return result;
    }

    // Wave type -> Opaque

    // Sound type -> Opaque

    // MusicData type -> Opaque

    // Head-Mounted-Display device parameters
    inline static VrDeviceInfo LuaGetArgument_VrDeviceInfo(lua_State *L, int index)
    {
        assert(0 && "Unsupported");
        /*
        VrDeviceInfo result = { 0 };
        index = lua_absindex(L, index); // Makes sure we use absolute indices because
        we push multiple values luaL_argcheck(L, lua_getfield(L, index, "hResolution")
        == LUA_TNUMBER, index, "Expected VrDeviceInfo.hResolution");
        result.hResolution = LuaGetArgument_int(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "vResolution") == LUA_TNUMBER, index,
        "Expected VrDeviceInfo.vResolution"); result.vResolution =
        LuaGetArgument_int(L, -1); luaL_argcheck(L, lua_getfield(L, index,
        "hScreenSize") == LUA_TNUMBER, index, "Expected VrDeviceInfo.hScreenSize");
        result.hScreenSize = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "vScreenSize") == LUA_TNUMBER, index,
        "Expected VrDeviceInfo.vScreenSize"); result.vScreenSize =
        LuaGetArgument_float(L, -1); luaL_argcheck(L, lua_getfield(L, index,
        "vScreenCenter") == LUA_TNUMBER, index, "Expected
        VrDeviceInfo.vScreenCenter"); result.vScreenCenter = LuaGetArgument_float(L,
        -1); luaL_argcheck(L, lua_getfield(L, index, "eyeToScreenDistance") ==
        LUA_TNUMBER, index, "Expected VrDeviceInfo.eyeToScreenDistance");
        result.eyeToScreenDistance = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "lensSeparationDistance") ==
        LUA_TNUMBER, index, "Expected VrDeviceInfo.lensSeparationDistance");
        result.lensSeparationDistance = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "interpupillaryDistance") ==
        LUA_TNUMBER, index, "Expected VrDeviceInfo.interpupillaryDistance");
        result.interpupillaryDistance = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "lensDistortionValues[4]") ==
        LUA_TNUMBER, index, "Expected VrDeviceInfo.lensDistortionValues[4]");
        result.lensDistortionValues[4] = LuaGetArgument_float(L, -1);
        luaL_argcheck(L, lua_getfield(L, index, "chromaAbCorrection[4]") ==
        LUA_TNUMBER, index, "Expected VrDeviceInfo.chromaAbCorrection[4]");
        result.chromaAbCorrection[4] = LuaGetArgument_float(L, -1);
        lua_pop(L, 10);
         */
        return (VrDeviceInfo){0};
    }

    //----------------------------------------------------------------------------------
    // LuaPush functions
    //----------------------------------------------------------------------------------
    inline static void LuaPush_Color(lua_State *L, Color color)
    {
        lua_createtable(L, 0, 4);
        LuaPush_int(L, color.r);
        lua_setfield(L, -2, "r");
        LuaPush_int(L, color.g);
        lua_setfield(L, -2, "g");
        LuaPush_int(L, color.b);
        lua_setfield(L, -2, "b");
        LuaPush_int(L, color.a);
        lua_setfield(L, -2, "a");
    }

    inline static void LuaPush_Vector2(lua_State *L, Vector2 vec)
    {
        lua_createtable(L, 0, 2);
        LuaPush_float(L, vec.x);
        lua_setfield(L, -2, "x");
        LuaPush_float(L, vec.y);
        lua_setfield(L, -2, "y");
    }

    inline static void LuaPush_Vector3(lua_State *L, Vector3 vec)
    {
        lua_createtable(L, 0, 3);
        LuaPush_float(L, vec.x);
        lua_setfield(L, -2, "x");
        LuaPush_float(L, vec.y);
        lua_setfield(L, -2, "y");
        LuaPush_float(L, vec.z);
        lua_setfield(L, -2, "z");
    }

    inline static void LuaPush_Vector4(lua_State *L, Vector4 vec)
    {
        lua_createtable(L, 0, 4);
        LuaPush_float(L, vec.x);
        lua_setfield(L, -2, "x");
        LuaPush_float(L, vec.y);
        lua_setfield(L, -2, "y");
        LuaPush_float(L, vec.z);
        lua_setfield(L, -2, "z");
        LuaPush_float(L, vec.w);
        lua_setfield(L, -2, "w");
    }

    inline static void LuaPush_Quaternion(lua_State *L, Quaternion vec)
    {
        lua_createtable(L, 0, 4);
        LuaPush_float(L, vec.x);
        lua_setfield(L, -2, "x");
        LuaPush_float(L, vec.y);
        lua_setfield(L, -2, "y");
        LuaPush_float(L, vec.z);
        lua_setfield(L, -2, "z");
        LuaPush_float(L, vec.w);
        lua_setfield(L, -2, "w");
    }

    inline static void LuaPush_Matrix(lua_State *L, Matrix *matrix)
    {
        int i;
        lua_createtable(L, 16, 0);
        float *num = (&matrix->m0);
        for (i = 0; i < 16; i++)
        {
            LuaPush_float(L, num[i]);
            lua_rawseti(L, -2, i + 1);
        }
    }

    inline static void LuaPush_Rectangle(lua_State *L, Rectangle rect)
    {
        lua_createtable(L, 0, 4);
        LuaPush_int(L, rect.x);
        lua_setfield(L, -2, "x");
        LuaPush_int(L, rect.y);
        lua_setfield(L, -2, "y");
        LuaPush_int(L, rect.width);
        lua_setfield(L, -2, "width");
        LuaPush_int(L, rect.height);
        lua_setfield(L, -2, "height");
    }

    inline static void LuaPush_Ray(lua_State *L, Ray ray)
    {
        lua_createtable(L, 0, 2);
        LuaPush_Vector3(L, ray.position);
        lua_setfield(L, -2, "position");
        LuaPush_Vector3(L, ray.direction);
        lua_setfield(L, -2, "direction");
    }

    inline static void LuaPush_RayCollision(lua_State *L, RayCollision hit)
    {
        lua_createtable(L, 0, 4);
        LuaPush_int(L, hit.hit);
        lua_setfield(L, -2, "hit");
        LuaPush_float(L, hit.distance);
        lua_setfield(L, -2, "distance");
        LuaPush_Vector3(L, hit.point);
        lua_setfield(L, -2, "point");
        LuaPush_Vector3(L, hit.normal);
        lua_setfield(L, -2, "normal");
    }

    inline static void LuaPush_BoundingBox(lua_State *L, BoundingBox bb)
    {
        lua_createtable(L, 0, 2);
        LuaPush_Vector3(L, bb.min);
        lua_setfield(L, -2, "min");
        LuaPush_Vector3(L, bb.max);
        lua_setfield(L, -2, "max");
    }

    inline static void LuaPush_Camera(lua_State *L, Camera cam)
    {
        lua_createtable(L, 0, 4);
        LuaPush_Vector3(L, cam.position);
        lua_setfield(L, -2, "position");
        LuaPush_Vector3(L, cam.target);
        lua_setfield(L, -2, "target");
        LuaPush_Vector3(L, cam.up);
        lua_setfield(L, -2, "up");
        LuaPush_float(L, cam.fovy);
        lua_setfield(L, -2, "fovy");
    }

    inline static void LuaPush_Camera2D(lua_State *L, Camera2D cam)
    {
        lua_createtable(L, 0, 4);
        LuaPush_Vector2(L, cam.offset);
        lua_setfield(L, -2, "offset");
        LuaPush_Vector2(L, cam.target);
        lua_setfield(L, -2, "target");
        LuaPush_float(L, cam.rotation);
        lua_setfield(L, -2, "rotation");
        LuaPush_float(L, cam.zoom);
        lua_setfield(L, -2, "zoom");
    }

    // REVIEW!!!
    /*
    static void LuaPush_Material(lua_State* L, Material mat)
    {
        lua_createtable(L, 0, 8);
        LuaPush_Shader(L, mat.shader);
        lua_setfield(L, -2, "shader");
        LuaPush_Texture2D(L, mat.texDiffuse);
        lua_setfield(L, -2, "texDiffuse");
        LuaPush_Texture2D(L, mat.texNormal);
        lua_setfield(L, -2, "texNormal");
        LuaPush_Texture2D(L, mat.texSpecular);
        lua_setfield(L, -2, "texSpecular");
        LuaPush_Color(L, mat.colDiffuse);
        lua_setfield(L, -2, "colDiffuse");
        LuaPush_Color(L, mat.colAmbient);
        lua_setfield(L, -2, "colAmbient");
        LuaPush_Color(L, mat.colSpecular);
        lua_setfield(L, -2, "colSpecular");
        LuaPush_float(L, mat.glossiness);
        lua_setfield(L, -2, "glossiness");
    }
    */

    inline static void LuaPush_Model(lua_State *L, Model mdl)
    {
        assert(0 && "Unsupported");
        /*
        lua_createtable(L, 0, 3);
        LuaPush_Mesh(L, mdl.mesh);
        lua_setfield(L, -2, "mesh");
        LuaPush_Matrix(L, &mdl.transform);
        lua_setfield(L, -2, "transform");
        LuaPush_Material(L, mdl.material);
        lua_setfield(L, -2, "material");
         */
    }

    //----------------------------------------------------------------------------------
    // raylib Lua Structure constructors
    //----------------------------------------------------------------------------------
    inline static int lua_Color(lua_State *L)
    {
        LuaPush_Color(L, (Color){(unsigned char)luaL_checkinteger(L, 1), (unsigned char)luaL_checkinteger(L, 2),
                                 (unsigned char)luaL_checkinteger(L, 3), (unsigned char)luaL_checkinteger(L, 4)});
        return 1;
    }

    inline static int lua_Vector2(lua_State *L)
    {
        LuaPush_Vector2(L, (Vector2){(float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2)});
        return 1;
    }

    inline static int lua_Vector3(lua_State *L)
    {
        LuaPush_Vector3(
            L, (Vector3){(float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3)});
        return 1;
    }

    inline static int lua_Vector4(lua_State *L)
    {
        LuaPush_Vector4(L, (Vector4){(float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2),
                                     (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4)});
        return 1;
    }

    inline static int lua_Quaternion(lua_State *L)
    {
        LuaPush_Quaternion(L, (Quaternion){(float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2),
                                           (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4)});
        return 1;
    }

    inline static int lua_Rectangle(lua_State *L)
    {
        LuaPush_Rectangle(L, (Rectangle){(float)luaL_checknumber(L, 1), (float)luaL_checknumber(L, 2),
                                         (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4)});
        return 1;
    }

    inline static int lua_Ray(lua_State *L)
    {
        Vector3 pos = LuaGetArgument_Vector3(L, 1);
        Vector3 dir = LuaGetArgument_Vector3(L, 2);
        LuaPush_Ray(L, (Ray){{pos.x, pos.y, pos.z}, {dir.x, dir.y, dir.z}});
        return 1;
    }

    inline static int lua_RayHitInfo(lua_State *L)
    {
        bool hit = LuaGetArgument_bool(L, 1);
        float dis = LuaGetArgument_float(L, 2);
        Vector3 pos = LuaGetArgument_Vector3(L, 3);
        Vector3 norm = LuaGetArgument_Vector3(L, 4);
        LuaPush_RayCollision(L, (RayCollision){hit, dis, {pos.x, pos.y, pos.z}, {norm.x, norm.y, norm.z}});
        return 1;
    }

    inline static int lua_BoundingBox(lua_State *L)
    {
        Vector3 min = LuaGetArgument_Vector3(L, 1);
        Vector3 max = LuaGetArgument_Vector3(L, 2);
        LuaPush_BoundingBox(L, (BoundingBox){{min.x, min.y, min.z}, {max.x, max.y, max.z}});
        return 1;
    }

    inline static int lua_Camera(lua_State *L)
    {
        Vector3 pos = LuaGetArgument_Vector3(L, 1);
        Vector3 tar = LuaGetArgument_Vector3(L, 2);
        Vector3 up = LuaGetArgument_Vector3(L, 3);
        float fovy = LuaGetArgument_float(L, 4);
        LuaPush_Camera(L, (Camera){{pos.x, pos.y, pos.z}, {tar.x, tar.y, tar.z}, {up.x, up.y, up.z}, fovy});
        return 1;
    }

    inline static int lua_Camera2D(lua_State *L)
    {
        Vector2 off = LuaGetArgument_Vector2(L, 1);
        Vector2 tar = LuaGetArgument_Vector2(L, 2);
        float rot = LuaGetArgument_float(L, 3);
        float zoom = LuaGetArgument_float(L, 4);
        LuaPush_Camera2D(L, (Camera2D){{off.x, off.y}, {tar.x, tar.y}, rot, zoom});
        return 1;
    }

    /*************************************************************************************
     *
     *  raylib Lua Functions Bindings
     *
     **************************************************************************************/

    //------------------------------------------------------------------------------------
    // raylib [core] module functions - Window and Graphics Device
    //------------------------------------------------------------------------------------

    // Initialize window and OpenGL context
    inline static int lua_InitWindow(lua_State *L)
    {
        int width = LuaGetArgument_int(L, 1);
        int height = LuaGetArgument_int(L, 2);
        const char *title = LuaGetArgument_string(L, 3);
        InitWindow(width, height, title);
        return 0;
    }

    // Close window and unload OpenGL context
    inline static int lua_CloseWindow(lua_State *L)
    {
        CloseWindow();
        return 0;
    }

    // Check if window has been initialized successfully
    inline static int lua_IsWindowReady(lua_State *L)
    {
        bool result = IsWindowReady();
        LuaPush_bool(L, result);
        return 1;
    }

    // Check if KEY_ESCAPE pressed or Close icon pressed
    inline static int lua_WindowShouldClose(lua_State *L)
    {
        bool result = WindowShouldClose();
        LuaPush_bool(L, result);
        return 1;
    }

    // Check if window has been minimized (or lost focus)
    inline static int lua_IsWindowMinimized(lua_State *L)
    {
        bool result = IsWindowMinimized();
        LuaPush_bool(L, result);
        return 1;
    }

    // Toggle fullscreen mode (only PLATFORM_DESKTOP)
    inline static int lua_ToggleFullscreen(lua_State *L)
    {
        ToggleFullscreen();
        return 0;
    }

    // Set icon for window (only PLATFORM_DESKTOP)
    inline static int lua_SetWindowIcon(lua_State *L)
    {
        Image image = LuaGetArgument_Image(L, 1);
        SetWindowIcon(image);
        return 0;
    }

    // Set title for window (only PLATFORM_DESKTOP)
    inline static int lua_SetWindowTitle(lua_State *L)
    {
        const char *title = LuaGetArgument_string(L, 1);
        SetWindowTitle(title);
        return 0;
    }

    // Set window position on screen (only PLATFORM_DESKTOP)
    inline static int lua_SetWindowPosition(lua_State *L)
    {
        int x = LuaGetArgument_int(L, 1);
        int y = LuaGetArgument_int(L, 2);
        SetWindowPosition(x, y);
        return 0;
    }

    // Set monitor for the current window (fullscreen mode)
    inline static int lua_SetWindowMonitor(lua_State *L)
    {
        int monitor = LuaGetArgument_int(L, 1);
        SetWindowMonitor(monitor);
        return 0;
    }

    // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
    inline static int lua_SetWindowMinSize(lua_State *L)
    {
        int width = LuaGetArgument_int(L, 1);
        int height = LuaGetArgument_int(L, 2);
        SetWindowMinSize(width, height);
        return 0;
    }

    // Set window dimensions
    inline static int lua_SetWindowSize(lua_State *L)
    {
        int width = LuaGetArgument_int(L, 1);
        int height = LuaGetArgument_int(L, 2);
        SetWindowSize(width, height);
        return 0;
    }

    // Get current screen width
    inline static int lua_GetScreenWidth(lua_State *L)
    {
        int result = GetScreenWidth();
        LuaPush_int(L, result);
        return 1;
    }

    // Get current screen height
    inline static int lua_GetScreenHeight(lua_State *L)
    {
        int result = GetScreenHeight();
        LuaPush_int(L, result);
        return 1;
    }

    // Cursor-related functions
    // Shows cursor
    inline static int lua_ShowCursor(lua_State *L)
    {
        ShowCursor();
        return 0;
    }

    // Hides cursor
    inline static int lua_HideCursor(lua_State *L)
    {
        HideCursor();
        return 0;
    }

    // Check if cursor is not visible
    inline static int lua_IsCursorHidden(lua_State *L)
    {
        bool result = IsCursorHidden();
        LuaPush_bool(L, result);
        return 1;
    }

    // Enables cursor (unlock cursor)
    inline static int lua_EnableCursor(lua_State *L)
    {
        EnableCursor();
        return 0;
    }

    // Disables cursor (lock cursor)
    inline static int lua_DisableCursor(lua_State *L)
    {
        DisableCursor();
        return 0;
    }

    // Drawing-related functions
    // Set background color (framebuffer clear color)
    inline static int lua_ClearBackground(lua_State *L)
    {
        Color color = LuaGetArgument_Color(L, 1);
        ClearBackground(color);
        return 0;
    }

    // Setup canvas (framebuffer) to start drawing
    inline static int lua_BeginDrawing(lua_State *L)
    {
        BeginDrawing();
        return 0;
    }

    // End canvas drawing and swap buffers (double buffering)
    inline static int lua_EndDrawing(lua_State *L)
    {
        EndDrawing();
        return 0;
    }

    // Initialize 2D mode with custom camera (2D)
    inline static int lua_BeginMode2D(lua_State *L)
    {
        Camera2D camera = LuaGetArgument_Camera2D(L, 1);
        BeginMode2D(camera);
        return 0;
    }

    // Ends 2D mode with custom camera
    inline static int lua_EndMode2D(lua_State *L)
    {
        EndMode2D();
        return 0;
    }

    // Initializes 3D mode with custom camera (3D)
    inline static int lua_BeginMode3D(lua_State *L)
    {
        assert(0 && "not implemented");
        /*
        Camera3D camera = LuaGetArgument_Camera3D(L, 1);
        BeginMode3D(camera);
         */
        return 0;
    }

    // Ends 3D mode and returns to default 2D orthographic mode
    inline static int lua_EndMode3D(lua_State *L)
    {
        EndMode3D();
        return 0;
    }

    // Initializes render texture for drawing
    inline static int lua_BeginTextureMode(lua_State *L)
    {
        RenderTexture2D target = LuaGetArgument_RenderTexture2D(L, 1);
        BeginTextureMode(target);
        return 0;
    }

    // Ends drawing to render texture
    inline static int lua_EndTextureMode(lua_State *L)
    {
        EndTextureMode();
        return 0;
    }

    // Screen-space-related functions
    // Returns a ray trace from mouse position
    inline static int lua_GetMouseRay(lua_State *L)
    {
        Vector2 mousePosition = LuaGetArgument_Vector2(L, 1);
        Camera camera = LuaGetArgument_Camera(L, 2);
        Ray result = GetMouseRay(mousePosition, camera);
        LuaPush_Ray(L, result);
        return 1;
    }

    // Returns the screen space position for a 3d world space position
    inline static int lua_GetWorldToScreen(lua_State *L)
    {
        Vector3 position = LuaGetArgument_Vector3(L, 1);
        Camera camera = LuaGetArgument_Camera(L, 2);
        Vector2 result = GetWorldToScreen(position, camera);
        LuaPush_Vector2(L, result);
        return 1;
    }

    // Returns camera transform matrix (view matrix)
    inline static int lua_GetCameraMatrix(lua_State *L)
    {
        Camera camera = LuaGetArgument_Camera(L, 1);
        Matrix result = GetCameraMatrix(camera);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    // Timming-related functions
    // Set target FPS (maximum)
    inline static int lua_SetTargetFPS(lua_State *L)
    {
        int fps = LuaGetArgument_int(L, 1);
        SetTargetFPS(fps);
        return 0;
    }

    // Returns current FPS
    inline static int lua_GetFPS(lua_State *L)
    {
        int result = GetFPS();
        LuaPush_int(L, result);
        return 1;
    }

    // Returns time in seconds for last frame drawn
    inline static int lua_GetFrameTime(lua_State *L)
    {
        float result = GetFrameTime();
        LuaPush_float(L, result);
        return 1;
    }

    // Returns elapsed time in seconds since InitWindow()
    inline static int lua_GetTime(lua_State *L)
    {
        float result = (float)GetTime();
        LuaPush_float(L, result);
        return 1;
    }

    // Color-related functions
    // Returns hexadecimal value for a Color
    inline static int lua_ColorToInt(lua_State *L)
    {
        Color color = LuaGetArgument_Color(L, 1);
        int result = ColorToInt(color);
        LuaPush_int(L, result);
        return 1;
    }

    inline static int lua_ColorAlpha(lua_State *L)
    {
        Color color = LuaGetArgument_Color(L, 1);
        color = ColorAlpha(color, LuaGetArgument_float(L, 2));
        LuaPush_Color(L, color);
        return 1;
    }

    inline static int lua_ColorBrightness(lua_State *L)
    {
        Color color = LuaGetArgument_Color(L, 1);
        color = ColorBrightness(color, LuaGetArgument_float(L, 2));
        LuaPush_Color(L, color);
        return 1;
    }

    // Returns color normalized as float [0..1]
    inline static int lua_ColorNormalize(lua_State *L)
    {
        Color color = LuaGetArgument_Color(L, 1);
        Vector4 result = ColorNormalize(color);
        LuaPush_Vector4(L, result);
        return 1;
    }

    // Returns color from normalized Vector4
    inline static int lua_ColorFromNormalized(lua_State *L)
    {
        Vector4 v4 = LuaGetArgument_Vector4(L, 1);
        Color col = ColorFromNormalized(v4);
        LuaPush_Color(L, col);
        return 1;
    }

    // Returns HSV values for a Color
    inline static int lua_ColorToHSV(lua_State *L)
    {
        Color color = LuaGetArgument_Color(L, 1);
        Vector3 result = ColorToHSV(color);
        LuaPush_Vector3(L, result);
        return 1;
    }

    // Returns a Color from HSV values
    inline static int lua_ColorLerp(lua_State *L)
    {
        Color c1 = LuaGetArgument_Color(L, 1);
        Color c2 = LuaGetArgument_Color(L, 2);
        float amount = LuaGetArgument_float(L, 3);
        Color result = ColorLerp(c1, c2, amount);
        LuaPush_Color(L, result);
        return 1;
    }

    // Returns a Color struct from hexadecimal value
    inline static int lua_GetColor(lua_State *L)
    {
        int hexValue = LuaGetArgument_int(L, 1);
        Color result = GetColor(hexValue);
        LuaPush_Color(L, result);
        return 1;
    }

    // Color fade-in or fade-out, alpha goes from 0.0f to 1.0f
    inline static int lua_Fade(lua_State *L)
    {
        Color color = LuaGetArgument_Color(L, 1);
        float alpha = LuaGetArgument_float(L, 2);
        Color result = Fade(color, alpha);
        LuaPush_Color(L, result);
        return 1;
    }

    // Misc. functions

    // Setup window configuration flags (view FLAGS)
    inline static int lua_SetConfigFlags(lua_State *L)
    {
        unsigned char flags = LuaGetArgument_unsigned(L, 1);
        SetConfigFlags(flags);
        return 0;
    }

    // Enable trace log message types (bit flags based)
    inline static int lua_SetTraceLog(lua_State *L)
    {
        unsigned char types = LuaGetArgument_unsigned(L, 1);
        SetTraceLogLevel(types);
        return 0;
    }

    /*
    #if defined(PLATFORM_WEB)
    static int LuaDrawLoopFunc;

    static void LuaDrawLoop()
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, LuaDrawLoopFunc);
        lua_call(L, 0, 0);
    }

    inline static int lua_SetDrawingLoop(lua_State* L)
    {
        luaL_argcheck(L, lua_isfunction(L, 1), 1, "Loop function expected");
        lua_pushvalue(L, 1);
        LuaDrawLoopFunc = luaL_ref(L, LUA_REGISTRYINDEX);
        SetDrawingLoop(&LuaDrawLoop);
        return 0;
    }
    #else
    // Set target FPS (maximum)
    inline static int lua_SetTargetFPS(lua_State* L)
    {
        int arg1 = LuaGetArgument_int(L, 1);
        SetTargetFPS(arg1);
        return 0;
    }
    #endif
    */

    /*
    // Converts Color to float array and normalizes
    inline static int lua_ColorToFloat(lua_State* L)
    {
        Color arg1 = LuaGetArgument_Color(L, 1);
        float *result = ColorToFloat(arg1);
        lua_createtable(L, 4, 0);
        for (int i = 0; i < 4; i++)
        {
            LuaPush_float(L, result[i]);
            lua_rawseti(L, -2, i + 1);
        }
        free(result);
        return 1;
    }

    // Converts Vector3 to float array
    inline static int lua_VectorToFloat(lua_State* L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        float *result = VectorToFloat(arg1);
        lua_createtable(L, 3, 0);
        for (int i = 0; i < 3; i++)
        {
            LuaPush_float(L, result[i]);
            lua_rawseti(L, -2, i + 1);
        }
        free(result);
        return 1;
    }

    // Converts Matrix to float array
    inline static int lua_MatrixToFloat(lua_State* L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        float *result = MatrixToFloat(arg1);
        lua_createtable(L, 16, 0);
        for (int i = 0; i < 16; i++)
        {
            LuaPush_float(L, result[i]);
            lua_rawseti(L, -2, i + 1);
        }
        free(result);
        return 1;
    }
    */

    // Returns a random value between min and max (both included)
    inline static int lua_GetRandomValue(lua_State *L)
    {
        int min = LuaGetArgument_int(L, 1);
        int max = LuaGetArgument_int(L, 2);
        int result = GetRandomValue(min, max);
        LuaPush_int(L, result);
        return 1;
    }

    // Detect if a key has been pressed once
    inline static int lua_IsKeyPressed(lua_State *L)
    {
        int key = LuaGetArgument_int(L, 1);
        bool result = IsKeyPressed(key);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect if a key is being pressed
    inline static int lua_IsKeyDown(lua_State *L)
    {
        int key = LuaGetArgument_int(L, 1);
        bool result = IsKeyDown(key);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect if a key has been released once
    inline static int lua_IsKeyReleased(lua_State *L)
    {
        int key = LuaGetArgument_int(L, 1);
        bool result = IsKeyReleased(key);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect if a key is NOT being pressed
    inline static int lua_IsKeyUp(lua_State *L)
    {
        int key = LuaGetArgument_int(L, 1);
        bool result = IsKeyUp(key);
        LuaPush_bool(L, result);
        return 1;
    }

    // Get latest key pressed
    inline static int lua_GetKeyPressed(lua_State *L)
    {
        int result = GetKeyPressed();
        LuaPush_int(L, result);
        return 1;
    }

    // Get last char pressed (ONLY ASCII SUPPORTED!!!)
    inline static int lua_GetCharPressed(lua_State *L)
    {
        int codepoint = GetCharPressed();
        if (codepoint != 0)
        {
            char str[] = {(char)codepoint, '\0'};
            lua_pushstring(L, str);
        }
        else
        {
            lua_pushstring(L, "");
        }
        return 1;
    }

    // Set a custom key to exit program (default is ESC)
    inline static int lua_SetExitKey(lua_State *L)
    {
        int key = LuaGetArgument_int(L, 1);
        SetExitKey(key);
        return 0;
    }

    // Input-related functions: gamepads
    // Detect if a gamepad is available
    inline static int lua_IsGamepadAvailable(lua_State *L)
    {
        int gamepad = LuaGetArgument_int(L, 1);
        bool result = IsGamepadAvailable(gamepad);
        LuaPush_bool(L, result);
        return 1;
    }

    // Input-related functions: mouse
    // Detect if a mouse button has been pressed once
    inline static int lua_IsMouseButtonPressed(lua_State *L)
    {
        int button = LuaGetArgument_int(L, 1);
        bool result = IsMouseButtonPressed(button);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect if a mouse button is being pressed
    inline static int lua_IsMouseButtonDown(lua_State *L)
    {
        int button = LuaGetArgument_int(L, 1);
        bool result = IsMouseButtonDown(button);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect if a mouse button has been released once
    inline static int lua_IsMouseButtonReleased(lua_State *L)
    {
        int button = LuaGetArgument_int(L, 1);
        bool result = IsMouseButtonReleased(button);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect if a mouse button is NOT being pressed
    inline static int lua_IsMouseButtonUp(lua_State *L)
    {
        int button = LuaGetArgument_int(L, 1);
        bool result = IsMouseButtonUp(button);
        LuaPush_bool(L, result);
        return 1;
    }

    // Returns mouse position X
    inline static int lua_GetMouseX(lua_State *L)
    {
        int result = GetMouseX();
        LuaPush_int(L, result);
        return 1;
    }

    // Returns mouse position Y
    inline static int lua_GetMouseY(lua_State *L)
    {
        int result = GetMouseY();
        LuaPush_int(L, result);
        return 1;
    }

    // Returns mouse position XY
    inline static int lua_GetMousePosition(lua_State *L)
    {
        Vector2 result = GetMousePosition();
        LuaPush_Vector2(L, result);
        return 1;
    }

    // Set mouse position XY
    inline static int lua_SetMousePosition(lua_State *L)
    {
        Vector2 position = LuaGetArgument_Vector2(L, 1);
        SetMousePosition(position.x, position.y);
        return 0;
    }

    // Set mouse scaling
    inline static int lua_SetMouseScale(lua_State *L)
    {
        assert(0 && "unsupported");
        /*
        float scale = LuaGetArgument_float(L, 1);
        SetMouseScale(scale);
         */
        return 0;
    }

    // Returns mouse wheel movement Y
    inline static int lua_GetMouseWheelMove(lua_State *L)
    {
        int result = GetMouseWheelMove();
        LuaPush_int(L, result);
        return 1;
    }

    // Input-related functions: touch
    // Returns touch position X for touch point 0 (relative to screen size)
    inline static int lua_GetTouchX(lua_State *L)
    {
        int result = GetTouchX();
        LuaPush_int(L, result);
        return 1;
    }

    // Returns touch position Y for touch point 0 (relative to screen size)
    inline static int lua_GetTouchY(lua_State *L)
    {
        int result = GetTouchY();
        LuaPush_int(L, result);
        return 1;
    }

    // Returns touch position XY for a touch point index (relative to screen size)
    inline static int lua_GetTouchPosition(lua_State *L)
    {
        int index = LuaGetArgument_int(L, 1);
        Vector2 result = GetTouchPosition(index);
        LuaPush_Vector2(L, result);
        return 1;
    }

    //------------------------------------------------------------------------------------
    // raylib [gestures] module functions - Gestures and Touch Handling
    //------------------------------------------------------------------------------------

    // Enable a set of gestures using flags
    inline static int lua_SetGesturesEnabled(lua_State *L)
    {
        unsigned int gestureFlags = LuaGetArgument_unsigned(L, 1);
        SetGesturesEnabled(gestureFlags);
        return 0;
    }

    // Check if a gesture have been detected
    inline static int lua_IsGestureDetected(lua_State *L)
    {
        int gesture = LuaGetArgument_int(L, 1);
        bool result = IsGestureDetected(gesture);
        LuaPush_bool(L, result);
        return 1;
    }

    // Get latest detected gesture
    inline static int lua_GetGestureDetected(lua_State *L)
    {
        int result = GetGestureDetected();
        LuaPush_int(L, result);
        return 1;
    }

    // Get touch points count
    inline static int lua_GetTouchPointsCount(lua_State *L)
    {
        int result = GetTouchPointCount();
        LuaPush_int(L, result);
        return 1;
    }

    // Get gesture hold time in milliseconds
    inline static int lua_GetGestureHoldDuration(lua_State *L)
    {
        float result = GetGestureHoldDuration();
        LuaPush_float(L, result);
        return 1;
    }

    // Get gesture drag vector
    inline static int lua_GetGestureDragVector(lua_State *L)
    {
        Vector2 result = GetGestureDragVector();
        LuaPush_Vector2(L, result);
        return 1;
    }

    // Get gesture drag angle
    inline static int lua_GetGestureDragAngle(lua_State *L)
    {
        float result = GetGestureDragAngle();
        LuaPush_float(L, result);
        return 1;
    }

    // Get gesture pinch delta
    inline static int lua_GetGesturePinchVector(lua_State *L)
    {
        Vector2 result = GetGesturePinchVector();
        LuaPush_Vector2(L, result);
        return 1;
    }

    // Get gesture pinch angle
    inline static int lua_GetGesturePinchAngle(lua_State *L)
    {
        float result = GetGesturePinchAngle();
        LuaPush_float(L, result);
        return 1;
    }

    //------------------------------------------------------------------------------------
    // raylib [camera] module functions - Camera System
    //------------------------------------------------------------------------------------

    // Set camera mode (multiple camera modes available)
    inline static int lua_SetCameraMode(lua_State *L)
    {
        assert(0 && "unsupported");
        /*
        Camera camera = LuaGetArgument_Camera(L, 1);
        int mode = LuaGetArgument_int(L, 2);
        SetCameraMode(camera, mode);
         */
        return 0;
    }

    // Update camera position for selected mode
    inline static int lua_UpdateCamera(lua_State *L)
    {
        assert(0 && "unsupported");
        /*
        Camera camera = LuaGetArgument_Camera(L, 1);
        UpdateCamera(camera);
         */
        return 0;
    }

    // Set camera pan key to combine with mouse movement (free camera)
    inline static int lua_SetCameraPanControl(lua_State *L)
    {
        assert(0 && "unsupported");
        /*
        int panKey = LuaGetArgument_int(L, 1);
        SetCameraPanControl(panKey);
         */
        return 0;
    }

    // Set camera alt key to combine with mouse movement (free camera)
    inline static int lua_SetCameraAltControl(lua_State *L)
    {
        assert(0 && "unsupported");
        /*
        int altKey = LuaGetArgument_int(L, 1);
        SetCameraAltControl(altKey);
         */
        return 0;
    }

    // Set camera smooth zoom key to combine with mouse (free camera)
    inline static int lua_SetCameraSmoothZoomControl(lua_State *L)
    {
        assert(0 && "unsupported");
        /*
        int szKey = LuaGetArgument_int(L, 1);
        SetCameraSmoothZoomControl(szKey);
         */
        return 0;
    }

    // Set camera move controls (1st person and 3rd person cameras)
    inline static int lua_SetCameraMoveControls(lua_State *L)
    {
        assert(0 && "unsupported");
        /*
        int frontKey = LuaGetArgument_int(L, 1);
        int backKey = LuaGetArgument_int(L, 2);
        int rightKey = LuaGetArgument_int(L, 3);
        int leftKey = LuaGetArgument_int(L, 4);
        int upKey = LuaGetArgument_int(L, 5);
        int downKey = LuaGetArgument_int(L, 6);
        SetCameraMoveControls(frontKey, backKey, rightKey, leftKey, upKey, downKey);
         */
        return 0;
    }

    //------------------------------------------------------------------------------------
    // raylib [shapes] module functions - Basic Shapes Drawing
    //------------------------------------------------------------------------------------

    // Draw a pixel
    inline static int lua_DrawPixel(lua_State *L)
    {
        int posX = LuaGetArgument_int(L, 1);
        int posY = LuaGetArgument_int(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawPixel(posX, posY, color);
        return 0;
    }

    // Draw a pixel (Vector version)
    inline static int lua_DrawPixelV(lua_State *L)
    {
        Vector2 position = LuaGetArgument_Vector2(L, 1);
        Color color = LuaGetArgument_Color(L, 2);
        DrawPixelV(position, color);
        return 0;
    }

    // Draw a line
    inline static int lua_DrawLine(lua_State *L)
    {
        int startPosX = LuaGetArgument_int(L, 1);
        int startPosY = LuaGetArgument_int(L, 2);
        int endPosX = LuaGetArgument_int(L, 3);
        int endPosY = LuaGetArgument_int(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawLine(startPosX, startPosY, endPosX, endPosY, color);
        return 0;
    }

    // Draw a line (Vector version)
    inline static int lua_DrawLineV(lua_State *L)
    {
        Vector2 startPos = LuaGetArgument_Vector2(L, 1);
        Vector2 endPos = LuaGetArgument_Vector2(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawLineV(startPos, endPos, color);
        return 0;
    }

    // Draw a line defining thickness
    inline static int lua_DrawLineEx(lua_State *L)
    {
        Vector2 startPos = LuaGetArgument_Vector2(L, 1);
        Vector2 endPos = LuaGetArgument_Vector2(L, 2);
        float thick = LuaGetArgument_float(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawLineEx(startPos, endPos, thick, color);
        return 0;
    }

    // Draw a line using cubic-bezier curves in-out
    inline static int lua_DrawLineBezier(lua_State *L)
    {
        Vector2 startPos = LuaGetArgument_Vector2(L, 1);
        Vector2 endPos = LuaGetArgument_Vector2(L, 2);
        float thick = LuaGetArgument_float(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawLineBezier(startPos, endPos, thick, color);
        return 0;
    }

    // Draw a color-filled circle
    inline static int lua_DrawCircle(lua_State *L)
    {
        int centerX = LuaGetArgument_int(L, 1);
        int centerY = LuaGetArgument_int(L, 2);
        float radius = LuaGetArgument_float(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawCircle(centerX, centerY, radius, color);
        return 0;
    }

    // Draw a gradient-filled circle
    inline static int lua_DrawCircleGradient(lua_State *L)
    {
        int centerX = LuaGetArgument_int(L, 1);
        int centerY = LuaGetArgument_int(L, 2);
        float radius = LuaGetArgument_float(L, 3);
        Color color1 = LuaGetArgument_Color(L, 4);
        Color color2 = LuaGetArgument_Color(L, 5);
        DrawCircleGradient(centerX, centerY, radius, color1, color2);
        return 0;
    }

    // Draw a color-filled circle (Vector version)
    inline static int lua_DrawCircleV(lua_State *L)
    {
        Vector2 center = LuaGetArgument_Vector2(L, 1);
        float radius = LuaGetArgument_float(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawCircleV(center, radius, color);
        return 0;
    }

    // Draw circle outline
    inline static int lua_DrawCircleLines(lua_State *L)
    {
        int centerX = LuaGetArgument_int(L, 1);
        int centerY = LuaGetArgument_int(L, 2);
        float radius = LuaGetArgument_float(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawCircleLines(centerX, centerY, radius, color);
        return 0;
    }

    // Draw a color-filled rectangle
    inline static int lua_DrawRectangle(lua_State *L)
    {
        int posX = LuaGetArgument_int(L, 1);
        int posY = LuaGetArgument_int(L, 2);
        int width = LuaGetArgument_int(L, 3);
        int height = LuaGetArgument_int(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawRectangle(posX, posY, width, height, color);
        return 0;
    }

    // Draw a color-filled rectangle (Vector version)
    inline static int lua_DrawRectangleV(lua_State *L)
    {
        Vector2 position = LuaGetArgument_Vector2(L, 1);
        Vector2 size = LuaGetArgument_Vector2(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawRectangleV(position, size, color);
        return 0;
    }

    // Draw a color-filled rectangle
    inline static int lua_DrawRectangleRec(lua_State *L)
    {
        Rectangle rec = LuaGetArgument_Rectangle(L, 1);
        Color color = LuaGetArgument_Color(L, 2);
        DrawRectangleRec(rec, color);
        return 0;
    }

    inline static int lua_DrawRectangleRounded(lua_State *L)
    {
        Rectangle rec = LuaGetArgument_Rectangle(L, 1);
        float roundness = LuaGetArgument_float(L, 2);
        int segments = LuaGetArgument_int(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawRectangleRounded(rec, roundness, segments, color);
        return 0;
    }

    inline static int lua_DrawRectangleRoundedLines(lua_State *L)
    {
        Rectangle rec = LuaGetArgument_Rectangle(L, 1);
        float roundness = LuaGetArgument_float(L, 2);
        int segments = LuaGetArgument_int(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawRectangleRoundedLines(rec, roundness, segments, color);
        return 0;
    }

    inline static int lua_DrawRectangleRoundedLinesEx(lua_State *L)
    {
        Rectangle rec = LuaGetArgument_Rectangle(L, 1);
        float roundness = LuaGetArgument_float(L, 2);
        int segments = LuaGetArgument_int(L, 3);
        float lineThick = LuaGetArgument_float(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawRectangleRoundedLinesEx(rec, roundness, segments, lineThick, color);
        return 0;
    }

    // Draw a color-filled rectangle with pro parameters
    inline static int lua_DrawRectanglePro(lua_State *L)
    {
        Rectangle rec = LuaGetArgument_Rectangle(L, 1);
        Vector2 origin = LuaGetArgument_Vector2(L, 2);
        float rotation = LuaGetArgument_float(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawRectanglePro(rec, origin, rotation, color);
        return 0;
    }

    // Draw a vertical-gradient-filled rectangle
    inline static int lua_DrawRectangleGradientV(lua_State *L)
    {
        int posX = LuaGetArgument_int(L, 1);
        int posY = LuaGetArgument_int(L, 2);
        int width = LuaGetArgument_int(L, 3);
        int height = LuaGetArgument_int(L, 4);
        Color color1 = LuaGetArgument_Color(L, 5);
        Color color2 = LuaGetArgument_Color(L, 6);
        DrawRectangleGradientV(posX, posY, width, height, color1, color2);
        return 0;
    }

    // Draw a horizontal-gradient-filled rectangle
    inline static int lua_DrawRectangleGradientH(lua_State *L)
    {
        int posX = LuaGetArgument_int(L, 1);
        int posY = LuaGetArgument_int(L, 2);
        int width = LuaGetArgument_int(L, 3);
        int height = LuaGetArgument_int(L, 4);
        Color color1 = LuaGetArgument_Color(L, 5);
        Color color2 = LuaGetArgument_Color(L, 6);
        DrawRectangleGradientH(posX, posY, width, height, color1, color2);
        return 0;
    }

    // Draw a gradient-filled rectangle with custom vertex colors
    inline static int lua_DrawRectangleGradientEx(lua_State *L)
    {
        Rectangle rec = LuaGetArgument_Rectangle(L, 1);
        Color col1 = LuaGetArgument_Color(L, 2);
        Color col2 = LuaGetArgument_Color(L, 3);
        Color col3 = LuaGetArgument_Color(L, 4);
        Color col4 = LuaGetArgument_Color(L, 5);
        DrawRectangleGradientEx(rec, col1, col2, col3, col4);
        return 0;
    }

    // Draw rectangle outline
    inline static int lua_DrawRectangleLines(lua_State *L)
    {
        int posX = LuaGetArgument_int(L, 1);
        int posY = LuaGetArgument_int(L, 2);
        int width = LuaGetArgument_int(L, 3);
        int height = LuaGetArgument_int(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawRectangleLines(posX, posY, width, height, color);
        return 0;
    }

    // Draw rectangle outline with extended parameters
    inline static int lua_DrawRectangleLinesEx(lua_State *L)
    {
        Rectangle rec = LuaGetArgument_Rectangle(L, 1);
        int lineThick = LuaGetArgument_int(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawRectangleLinesEx(rec, lineThick, color);
        return 0;
    }

    // Draw a color-filled triangle
    inline static int lua_DrawTriangle(lua_State *L)
    {
        Vector2 v1 = LuaGetArgument_Vector2(L, 1);
        Vector2 v2 = LuaGetArgument_Vector2(L, 2);
        Vector2 v3 = LuaGetArgument_Vector2(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawTriangle(v1, v2, v3, color);
        return 0;
    }

    // Draw triangle outline
    inline static int lua_DrawTriangleLines(lua_State *L)
    {
        Vector2 v1 = LuaGetArgument_Vector2(L, 1);
        Vector2 v2 = LuaGetArgument_Vector2(L, 2);
        Vector2 v3 = LuaGetArgument_Vector2(L, 3);
        Color color = LuaGetArgument_Color(L, 4);
        DrawTriangleLines(v1, v2, v3, color);
        return 0;
    }

    // Draw a regular polygon (Vector version)
    inline static int lua_DrawPoly(lua_State *L)
    {
        Vector2 center = LuaGetArgument_Vector2(L, 1);
        int sides = LuaGetArgument_int(L, 2);
        float radius = LuaGetArgument_float(L, 3);
        float rotation = LuaGetArgument_float(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawPoly(center, sides, radius, rotation, color);
        return 0;
    }

    // Check collision between two rectangles
    inline static int lua_CheckCollisionRecs(lua_State *L)
    {
        Rectangle rec1 = LuaGetArgument_Rectangle(L, 1);
        Rectangle rec2 = LuaGetArgument_Rectangle(L, 2);
        bool result = CheckCollisionRecs(rec1, rec2);
        LuaPush_bool(L, result);
        return 1;
    }

    // Check collision between two circles
    inline static int lua_CheckCollisionCircles(lua_State *L)
    {
        Vector2 center1 = LuaGetArgument_Vector2(L, 1);
        float radius1 = LuaGetArgument_float(L, 2);
        Vector2 center2 = LuaGetArgument_Vector2(L, 3);
        float radius2 = LuaGetArgument_float(L, 4);
        bool result = CheckCollisionCircles(center1, radius1, center2, radius2);
        LuaPush_bool(L, result);
        return 1;
    }

    // Check collision between circle and rectangle
    inline static int lua_CheckCollisionCircleRec(lua_State *L)
    {
        Vector2 center = LuaGetArgument_Vector2(L, 1);
        float radius = LuaGetArgument_float(L, 2);
        Rectangle rec = LuaGetArgument_Rectangle(L, 3);
        bool result = CheckCollisionCircleRec(center, radius, rec);
        LuaPush_bool(L, result);
        return 1;
    }

    // Get collision rectangle for two rectangles collision
    inline static int lua_GetCollisionRec(lua_State *L)
    {
        Rectangle rec1 = LuaGetArgument_Rectangle(L, 1);
        Rectangle rec2 = LuaGetArgument_Rectangle(L, 2);
        Rectangle result = GetCollisionRec(rec1, rec2);
        LuaPush_Rectangle(L, result);
        return 1;
    }

    // Check if point is inside rectangle
    inline static int lua_CheckCollisionPointRec(lua_State *L)
    {
        Vector2 point = LuaGetArgument_Vector2(L, 1);
        Rectangle rec = LuaGetArgument_Rectangle(L, 2);
        bool result = CheckCollisionPointRec(point, rec);
        LuaPush_bool(L, result);
        return 1;
    }

    // Check if point is inside circle
    inline static int lua_CheckCollisionPointCircle(lua_State *L)
    {
        Vector2 point = LuaGetArgument_Vector2(L, 1);
        Vector2 center = LuaGetArgument_Vector2(L, 2);
        float radius = LuaGetArgument_float(L, 3);
        bool result = CheckCollisionPointCircle(point, center, radius);
        LuaPush_bool(L, result);
        return 1;
    }

    // Check if point is inside a triangle
    inline static int lua_CheckCollisionPointTriangle(lua_State *L)
    {
        Vector2 point = LuaGetArgument_Vector2(L, 1);
        Vector2 p1 = LuaGetArgument_Vector2(L, 2);
        Vector2 p2 = LuaGetArgument_Vector2(L, 3);
        Vector2 p3 = LuaGetArgument_Vector2(L, 4);
        bool result = CheckCollisionPointTriangle(point, p1, p2, p3);
        LuaPush_bool(L, result);
        return 1;
    }

    //------------------------------------------------------------------------------------
    // raylib [textures] module functions - Texture Loading and Drawing
    //------------------------------------------------------------------------------------

    // Load texture from file into GPU memory (VRAM)
    inline static int lua_LoadTexture(lua_State *L)
    {
        const char *fileName = LuaGetArgument_string(L, 1);
        Texture2D result = LoadTexture(fileName);
        LuaPush_Texture2D(L, result);
        return 1;
    }

    // Load texture from image data
    inline static int lua_LoadTextureFromImage(lua_State *L)
    {
        Image image = LuaGetArgument_Image(L, 1);
        Texture2D result = LoadTextureFromImage(image);
        LuaPush_Texture2D(L, result);
        return 1;
    }

    // Load texture for rendering (framebuffer)
    inline static int lua_LoadRenderTexture(lua_State *L)
    {
        int width = LuaGetArgument_int(L, 1);
        int height = LuaGetArgument_int(L, 2);
        RenderTexture2D result = LoadRenderTexture(width, height);
        LuaPush_RenderTexture2D(L, result);
        return 1;
    }

    // Unload image from CPU memory (RAM)
    inline static int lua_UnloadImage(lua_State *L)
    {
        Image image = LuaGetArgument_Image(L, 1);
        UnloadImage(image);
        return 0;
    }

    // Unload texture from GPU memory (VRAM)
    inline static int lua_UnloadTexture(lua_State *L)
    {
        Texture2D texture = LuaGetArgument_Texture2D(L, 1);
        UnloadTexture(texture);
        return 0;
    }

    // Unload render texture from GPU memory (VRAM)
    inline static int lua_UnloadRenderTexture(lua_State *L)
    {
        RenderTexture2D target = LuaGetArgument_RenderTexture2D(L, 1);
        UnloadRenderTexture(target);
        return 0;
    }

    // WARNING: Update GPU texture with new data
    inline static int lua_UpdateTexture(lua_State *L)
    {
        // TODO: arg2 parameters is a void pointer...

        Texture2D arg1 = LuaGetArgument_Texture2D(L, 1);
        void *arg2 = LuaGetArgument_ptr(L, 2);
        UpdateTexture(arg1, arg2); // ISSUE: #2 string expected, got table ->
                                   // GetImageData() returns a table!
        return 0;
    }

    //----------------------------------------------------------------------------------
    // Image manipulation functions
    //----------------------------------------------------------------------------------

    // Create an image duplicate (useful for transformations)
    inline static int lua_ImageCopy(lua_State *L)
    {
        Image image = LuaGetArgument_Image(L, 1);
        Image result = ImageCopy(image);
        LuaPush_Image(L, result);
        return 1;
    }

    // Texture2D drawing functions

    // Draw a Texture2D
    inline static int lua_DrawTexture(lua_State *L)
    {
        Texture2D texture = LuaGetArgument_Texture2D(L, 1);
        int posX = LuaGetArgument_int(L, 2);
        int posY = LuaGetArgument_int(L, 3);
        Color tint = LuaGetArgument_Color(L, 4);
        DrawTexture(texture, posX, posY, tint);
        return 0;
    }

    // Draw a Texture2D with position defined as Vector2
    inline static int lua_DrawTextureV(lua_State *L)
    {
        Texture2D texture = LuaGetArgument_Texture2D(L, 1);
        Vector2 position = LuaGetArgument_Vector2(L, 2);
        Color tint = LuaGetArgument_Color(L, 3);
        DrawTextureV(texture, position, tint);
        return 0;
    }

    // Draw a Texture2D with extended parameters
    inline static int lua_DrawTextureEx(lua_State *L)
    {
        Texture2D texture = LuaGetArgument_Texture2D(L, 1);
        Vector2 position = LuaGetArgument_Vector2(L, 2);
        float rotation = LuaGetArgument_float(L, 3);
        float scale = LuaGetArgument_float(L, 4);
        Color tint = LuaGetArgument_Color(L, 5);
        DrawTextureEx(texture, position, rotation, scale, tint);
        return 0;
    }

    // Draw a part of a texture defined by a rectangle
    inline static int lua_DrawTextureRec(lua_State *L)
    {
        Texture2D texture = LuaGetArgument_Texture2D(L, 1);
        Rectangle sourceRec = LuaGetArgument_Rectangle(L, 2);
        Vector2 position = LuaGetArgument_Vector2(L, 3);
        Color tint = LuaGetArgument_Color(L, 4);
        DrawTextureRec(texture, sourceRec, position, tint);
        return 0;
    }

    // Draw a part of a texture defined by a rectangle with 'pro' parameters
    inline static int lua_DrawTexturePro(lua_State *L)
    {
        Texture2D texture = LuaGetArgument_Texture2D(L, 1);
        Rectangle sourceRec = LuaGetArgument_Rectangle(L, 2);
        Rectangle destRec = LuaGetArgument_Rectangle(L, 3);
        Vector2 origin = LuaGetArgument_Vector2(L, 4);
        float rotation = LuaGetArgument_float(L, 5);
        Color tint = LuaGetArgument_Color(L, 6);
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, tint);
        return 0;
    }

    //------------------------------------------------------------------------------------
    // raylib [text] module functions - Font Loading and Text Drawing
    //------------------------------------------------------------------------------------

    // Get the default Font
    inline static int lua_GetFontDefault(lua_State *L)
    {
        Font result = GetFontDefault();
        LuaPush_Font(L, result);
        return 1;
    }

    // Load font from file into GPU memory (VRAM)
    inline static int lua_LoadFont(lua_State *L)
    {
        const char *fileName = LuaGetArgument_string(L, 1);
        Font result = LoadFont(fileName);
        LuaPush_Font(L, result);
        return 1;
    }

    // Load font from file with extended parameters
    inline static int lua_LoadFontEx(lua_State *L)
    {
        const char *fileName = LuaGetArgument_string(L, 1);
        int fontSize = LuaGetArgument_int(L, 2);
        int charsCount = LuaGetArgument_int(L, 3);
        int fontChars = LuaGetArgument_int(L, 4);
        Font result = LoadFontEx(fileName, fontSize, &charsCount, fontChars);
        LuaPush_Font(L, result);
        return 1;
    }

    // Unload Font from GPU memory (VRAM)
    inline static int lua_UnloadFont(lua_State *L)
    {
        Font font = LuaGetArgument_Font(L, 1);
        UnloadFont(font);
        return 0;
    }

    // Text drawing functions
    // Shows current FPS
    inline static int lua_DrawFPS(lua_State *L)
    {
        int posX = LuaGetArgument_int(L, 1);
        int posY = LuaGetArgument_int(L, 2);
        DrawFPS(posX, posY);
        return 0;
    }

    // Draw text (using default font)
    inline static int lua_DrawText(lua_State *L)
    {
        const char *text = LuaGetArgument_string(L, 1);
        int posX = LuaGetArgument_int(L, 2);
        int posY = LuaGetArgument_int(L, 3);
        int fontSize = LuaGetArgument_int(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawText(text, posX, posY, fontSize, color);
        return 0;
    }

    // WARNING: Draw text using font and additional parameters
    inline static int lua_DrawTextEx(lua_State *L)
    {
        Font font = LuaGetArgument_Font(L, 1);
        const char *text = LuaGetArgument_string(L, 2);
        Vector2 position = LuaGetArgument_Vector2(L, 3);
        float fontSize = LuaGetArgument_float(L, 4);
        float spacing = LuaGetArgument_float(L, 5);
        Color tint = LuaGetArgument_Color(L, 6);
        DrawTextEx(font, text, position, fontSize, spacing, tint);
        return 0;
    }

    // Text misc. functions
    // Measure string width for default font
    inline static int lua_MeasureText(lua_State *L)
    {
        const char *text = LuaGetArgument_string(L, 1);
        int fontSize = LuaGetArgument_int(L, 2);
        int result = MeasureText(text, fontSize);
        LuaPush_int(L, result);
        return 1;
    }

    // Measure string size for Font
    inline static int lua_MeasureTextEx(lua_State *L)
    {
        Font font = LuaGetArgument_Font(L, 1);
        const char *text = LuaGetArgument_string(L, 2);
        float fontSize = LuaGetArgument_float(L, 3);
        float spacing = LuaGetArgument_float(L, 4);
        Vector2 result = MeasureTextEx(font, text, fontSize, spacing);
        LuaPush_Vector2(L, result);
        return 1;
    }

    // WARNING: FormatText() can be replaced by Lua function: string.format()
    // WARNING: SubText() can be replaced by Lua function: string.sub()

    // Get index position for a unicode character on font
    inline static int lua_GetGlyphIndex(lua_State *L)
    {
        Font font = LuaGetArgument_Font(L, 1);
        int character = LuaGetArgument_int(L, 2);
        int result = GetGlyphIndex(font, character);
        LuaPush_int(L, result);
        return 1;
    }

    //------------------------------------------------------------------------------------
    // raylib [models] module functions - Basic 3d Shapes Drawing Functions
    //------------------------------------------------------------------------------------

    // Draw a line in 3D world space
    inline static int lua_DrawLine3D(lua_State *L)
    {
        Vector3 startPos = LuaGetArgument_Vector3(L, 1);
        Vector3 endPos = LuaGetArgument_Vector3(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawLine3D(startPos, endPos, color);
        return 0;
    }

    // Draw a circle in 3D world space
    inline static int lua_DrawCircle3D(lua_State *L)
    {
        Vector3 center = LuaGetArgument_Vector3(L, 1);
        float radius = LuaGetArgument_float(L, 2);
        Vector3 rotationAxis = LuaGetArgument_Vector3(L, 3);
        float rotationAngle = LuaGetArgument_float(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawCircle3D(center, radius, rotationAxis, rotationAngle, color);
        return 0;
    }

    // Draw cube
    inline static int lua_DrawCube(lua_State *L)
    {
        Vector3 position = LuaGetArgument_Vector3(L, 1);
        float width = LuaGetArgument_float(L, 2);
        float height = LuaGetArgument_float(L, 3);
        float length = LuaGetArgument_float(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawCube(position, width, height, length, color);
        return 0;
    }

    // Draw cube (Vector version)
    inline static int lua_DrawCubeV(lua_State *L)
    {
        Vector3 position = LuaGetArgument_Vector3(L, 1);
        Vector3 size = LuaGetArgument_Vector3(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawCubeV(position, size, color);
        return 0;
    }

    // Draw cube wires
    inline static int lua_DrawCubeWires(lua_State *L)
    {
        Vector3 position = LuaGetArgument_Vector3(L, 1);
        float width = LuaGetArgument_float(L, 2);
        float height = LuaGetArgument_float(L, 3);
        float length = LuaGetArgument_float(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawCubeWires(position, width, height, length, color);
        return 0;
    }

    // Draw sphere
    inline static int lua_DrawSphere(lua_State *L)
    {
        Vector3 centerPos = LuaGetArgument_Vector3(L, 1);
        float radius = LuaGetArgument_float(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawSphere(centerPos, radius, color);
        return 0;
    }

    // Draw sphere with extended parameters
    inline static int lua_DrawSphereEx(lua_State *L)
    {
        Vector3 centerPos = LuaGetArgument_Vector3(L, 1);
        float radius = LuaGetArgument_float(L, 2);
        int rings = LuaGetArgument_int(L, 3);
        int slices = LuaGetArgument_int(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawSphereEx(centerPos, radius, rings, slices, color);
        return 0;
    }

    // Draw sphere wires
    inline static int lua_DrawSphereWires(lua_State *L)
    {
        Vector3 centerPos = LuaGetArgument_Vector3(L, 1);
        float radius = LuaGetArgument_float(L, 2);
        int rings = LuaGetArgument_int(L, 3);
        int slices = LuaGetArgument_int(L, 4);
        Color color = LuaGetArgument_Color(L, 5);
        DrawSphereWires(centerPos, radius, rings, slices, color);
        return 0;
    }

    // Draw a cylinder/cone
    inline static int lua_DrawCylinder(lua_State *L)
    {
        Vector3 position = LuaGetArgument_Vector3(L, 1);
        float radiusTop = LuaGetArgument_float(L, 2);
        float radiusBottom = LuaGetArgument_float(L, 3);
        float height = LuaGetArgument_float(L, 4);
        int slices = LuaGetArgument_int(L, 5);
        Color color = LuaGetArgument_Color(L, 6);
        DrawCylinder(position, radiusTop, radiusBottom, height, slices, color);
        return 0;
    }

    // Draw a cylinder/cone wires
    inline static int lua_DrawCylinderWires(lua_State *L)
    {
        Vector3 position = LuaGetArgument_Vector3(L, 1);
        float radiusTop = LuaGetArgument_float(L, 2);
        float radiusBottom = LuaGetArgument_float(L, 3);
        float height = LuaGetArgument_float(L, 4);
        int slices = LuaGetArgument_int(L, 5);
        Color color = LuaGetArgument_Color(L, 6);
        DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color);
        return 0;
    }

    // Draw a plane XZ
    inline static int lua_DrawPlane(lua_State *L)
    {
        Vector3 centerPos = LuaGetArgument_Vector3(L, 1);
        Vector2 size = LuaGetArgument_Vector2(L, 2);
        Color color = LuaGetArgument_Color(L, 3);
        DrawPlane(centerPos, size, color);
        return 0;
    }

    // Draw a ray line
    inline static int lua_DrawRay(lua_State *L)
    {
        Ray ray = LuaGetArgument_Ray(L, 1);
        Color color = LuaGetArgument_Color(L, 2);
        DrawRay(ray, color);
        return 0;
    }

    // Draw a grid (centered at (0, 0, 0))
    inline static int lua_DrawGrid(lua_State *L)
    {
        int slices = LuaGetArgument_int(L, 1);
        float spacing = LuaGetArgument_float(L, 2);
        DrawGrid(slices, spacing);
        return 0;
    }

    // Draw bounding box (wires)
    inline static int lua_DrawBoundingBox(lua_State *L)
    {
        BoundingBox box = LuaGetArgument_BoundingBox(L, 1);
        Color color = LuaGetArgument_Color(L, 2);
        DrawBoundingBox(box, color);
        return 0;
    }

    // Draw a billboard texture
    inline static int lua_DrawBillboard(lua_State *L)
    {
        Camera camera = LuaGetArgument_Camera(L, 1);
        Texture2D texture = LuaGetArgument_Texture2D(L, 2);
        Vector3 center = LuaGetArgument_Vector3(L, 3);
        float size = LuaGetArgument_float(L, 4);
        Color tint = LuaGetArgument_Color(L, 5);
        DrawBillboard(camera, texture, center, size, tint);
        return 0;
    }

    // Collision detection functions
    // Detect collision between two spheres
    inline static int lua_CheckCollisionSpheres(lua_State *L)
    {
        Vector3 centerA = LuaGetArgument_Vector3(L, 1);
        float radiusA = LuaGetArgument_float(L, 2);
        Vector3 centerB = LuaGetArgument_Vector3(L, 3);
        float radiusB = LuaGetArgument_float(L, 4);
        bool result = CheckCollisionSpheres(centerA, radiusA, centerB, radiusB);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect collision between two bounding boxes
    inline static int lua_CheckCollisionBoxes(lua_State *L)
    {
        BoundingBox box1 = LuaGetArgument_BoundingBox(L, 1);
        BoundingBox box2 = LuaGetArgument_BoundingBox(L, 2);
        bool result = CheckCollisionBoxes(box1, box2);
        LuaPush_bool(L, result);
        return 1;
    }

    // Detect collision between box and sphere
    inline static int lua_CheckCollisionBoxSphere(lua_State *L)
    {
        BoundingBox box = LuaGetArgument_BoundingBox(L, 1);
        Vector3 centerSphere = LuaGetArgument_Vector3(L, 2);
        float radiusSphere = LuaGetArgument_float(L, 3);
        bool result = CheckCollisionBoxSphere(box, centerSphere, radiusSphere);
        LuaPush_bool(L, result);
        return 1;
    }

    //------------------------------------------------------------------------------------
    // raylib [raymath] module functions - Shaders
    //------------------------------------------------------------------------------------

    // Load shader from files and bind default locations
    inline static int lua_LoadShader(lua_State *L)
    {
        const char *vsFileName = LuaGetArgument_string(L, 1);
        const char *fsFileName = LuaGetArgument_string(L, 2);
        Shader result = LoadShader(vsFileName, fsFileName);
        LuaPush_Shader(L, result);
        return 1;
    }

    inline static int lua_LoadShaderFromMemory(lua_State *L)
    {
        const char *vertex = LuaGetArgument_string(L, 1);
        const char *fragment = LuaGetArgument_string(L, 2);
        Shader result = LoadShaderFromMemory(vertex, fragment);
        LuaPush_Shader(L, result);
        return 1;
    }

    // Unload shader from GPU memory (VRAM)
    inline static int lua_UnloadShader(lua_State *L)
    {
        Shader shader = LuaGetArgument_Shader(L, 1);
        UnloadShader(shader);
        return 0;
    }

    // Shader configuration functions

    // Get shader uniform location
    inline static int lua_GetShaderLocation(lua_State *L)
    {
        Shader shader = LuaGetArgument_Shader(L, 1);
        const char *uniformName = LuaGetArgument_string(L, 2);
        int result = GetShaderLocation(shader, uniformName);
        LuaPush_int(L, result);
        return 1;
    }

    // Set shader uniform value (matrix 4x4)
    inline static int lua_SetShaderValueMatrix(lua_State *L)
    {
        Shader shader = LuaGetArgument_Shader(L, 1);
        int uniformLoc = LuaGetArgument_int(L, 2);
        Matrix mat = LuaGetArgument_Matrix(L, 3);
        SetShaderValueMatrix(shader, uniformLoc, mat);
        return 0;
    }

    // Shading begin/end functions
    // Begin custom shader drawing
    inline static int lua_BeginShaderMode(lua_State *L)
    {
        Shader shader = LuaGetArgument_Shader(L, 1);
        BeginShaderMode(shader);
        return 0;
    }

    // End custom shader drawing (use default shader)
    inline static int lua_EndShaderMode(lua_State *L)
    {
        EndShaderMode();
        return 0;
    }

    // Begin blending mode (alpha, additive, multiplied)
    inline static int lua_BeginBlendMode(lua_State *L)
    {
        int mode = LuaGetArgument_int(L, 1);
        BeginBlendMode(mode);
        return 0;
    }

    // End blending mode (reset to default: alpha blending)
    inline static int lua_EndBlendMode(lua_State *L)
    {
        EndBlendMode();
        return 0;
    }

    //------------------------------------------------------------------------------------
    // raylib [audio] module functions - Audio Loading and Playing
    //------------------------------------------------------------------------------------

    // Initialize audio device and context
    inline static int lua_InitAudioDevice(lua_State *L)
    {
        InitAudioDevice();
        return 0;
    }

    // Close the audio device and context
    inline static int lua_CloseAudioDevice(lua_State *L)
    {
        CloseAudioDevice();
        return 0;
    }

    // Check if audio device has been initialized successfully
    inline static int lua_IsAudioDeviceReady(lua_State *L)
    {
        bool result = IsAudioDeviceReady();
        LuaPush_bool(L, result);
        return 1;
    }

    // Set master volume (listener)
    inline static int lua_SetMasterVolume(lua_State *L)
    {
        float volume = LuaGetArgument_float(L, 1);
        SetMasterVolume(volume);
        return 0;
    }

    // Wave/Sound loading/unloading functions
    // Load wave data from file
    inline static int lua_LoadWave(lua_State *L)
    {
        const char *fileName = LuaGetArgument_string(L, 1);
        Wave result = LoadWave(fileName);
        LuaPush_Wave(L, result);
        return 1;
    }

    // Load sound from file
    inline static int lua_LoadSound(lua_State *L)
    {
        const char *fileName = LuaGetArgument_string(L, 1);
        Sound result = LoadSound(fileName);
        LuaPush_Sound(L, result);
        return 1;
    }

    // Load sound from wave data
    inline static int lua_LoadSoundFromWave(lua_State *L)
    {
        Wave wave = LuaGetArgument_Wave(L, 1);
        Sound result = LoadSoundFromWave(wave);
        LuaPush_Sound(L, result);
        return 1;
    }

    // WARNING: Update sound buffer with new data
    inline static int lua_UpdateSound(lua_State *L)
    {
        // TODO: arg2 parameter is a void pointer...

        Sound arg1 = LuaGetArgument_Sound(L, 1);
        void *arg2 = LuaGetArgument_ptr(L, 2);
        int arg3 = LuaGetArgument_int(L, 3);
        UpdateSound(arg1, arg2, arg3);
        return 0;
    }

    // Unload wave data
    inline static int lua_UnloadWave(lua_State *L)
    {
        Wave wave = LuaGetArgument_Wave(L, 1);
        UnloadWave(wave);
        return 0;
    }

    // Unload sound
    inline static int lua_UnloadSound(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        UnloadSound(sound);
        return 0;
    }

    // Wave/Sound management functions
    // Play a sound
    inline static int lua_PlaySound(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        PlaySound(sound);
        return 0;
    }

    // Pause a sound
    inline static int lua_PauseSound(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        PauseSound(sound);
        return 0;
    }

    // Resume a paused sound
    inline static int lua_ResumeSound(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        ResumeSound(sound);
        return 0;
    }

    // Stop playing a sound
    inline static int lua_StopSound(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        StopSound(sound);
        return 0;
    }

    // Check if a sound is currently playing
    inline static int lua_IsSoundPlaying(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        bool result = IsSoundPlaying(sound);
        LuaPush_bool(L, result);
        return 1;
    }

    // Set volume for a sound (1.0 is max level)
    inline static int lua_SetSoundVolume(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        float volume = LuaGetArgument_float(L, 2);
        SetSoundVolume(sound, volume);
        return 0;
    }

    // Set pitch for a sound (1.0 is base level)
    inline static int lua_SetSoundPitch(lua_State *L)
    {
        Sound sound = LuaGetArgument_Sound(L, 1);
        float pitch = LuaGetArgument_float(L, 2);
        SetSoundPitch(sound, pitch);
        return 0;
    }

    // Music management functions

    // Load music stream from file
    inline static int lua_LoadMusicStream(lua_State *L)
    {
        const char *fileName = LuaGetArgument_string(L, 1);
        Music result = LoadMusicStream(fileName);
        LuaPush_Music(L, result);
        return 1;
    }

    // Unload music stream
    inline static int lua_UnloadMusicStream(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        UnloadMusicStream(music);
        return 0;
    }

    // Start music playing
    inline static int lua_PlayMusicStream(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        PlayMusicStream(music);
        return 0;
    }

    // Updates buffers for music streaming
    inline static int lua_UpdateMusicStream(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        UpdateMusicStream(music);
        return 0;
    }

    // Stop music playing
    inline static int lua_StopMusicStream(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        StopMusicStream(music);
        return 0;
    }

    // Pause music playing
    inline static int lua_PauseMusicStream(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        PauseMusicStream(music);
        return 0;
    }

    // Resume playing paused music
    inline static int lua_ResumeMusicStream(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        ResumeMusicStream(music);
        return 0;
    }

    // Check if music is playing
    inline static int lua_IsMusicPlaying(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        bool result = IsMusicStreamPlaying(music);
        LuaPush_bool(L, result);
        return 1;
    }

    // Set volume for music (1.0 is max level)
    inline static int lua_SetMusicVolume(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        float volume = LuaGetArgument_float(L, 2);
        SetMusicVolume(music, volume);
        return 0;
    }

    // Set pitch for a music (1.0 is base level)
    inline static int lua_SetMusicPitch(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        float pitch = LuaGetArgument_float(L, 2);
        SetMusicPitch(music, pitch);
        return 0;
    }

    // Get music time length (in seconds)
    inline static int lua_GetMusicTimeLength(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        float result = GetMusicTimeLength(music);
        LuaPush_float(L, result);
        return 1;
    }

    // Get current music time played (in seconds)
    inline static int lua_GetMusicTimePlayed(lua_State *L)
    {
        Music music = LuaGetArgument_Music(L, 1);
        float result = GetMusicTimePlayed(music);
        LuaPush_float(L, result);
        return 1;
    }

    // AudioStream management functions
    // Init audio stream (to stream raw audio pcm data)
    inline static int lua_InitAudioStream(lua_State *L)
    {
        unsigned int sampleRate = LuaGetArgument_unsigned(L, 1);
        unsigned int sampleSize = LuaGetArgument_unsigned(L, 2);
        unsigned int channels = LuaGetArgument_unsigned(L, 3);
        AudioStream result = LoadAudioStream(sampleRate, sampleSize, channels);
        LuaPush_AudioStream(L, result);
        return 1;
    }

    // WARNING: Update audio stream buffers with data
    inline static int lua_UpdateAudioStream(lua_State *L)
    {
        // TODO: arg2 parameter is a void pointer...

        AudioStream arg1 = LuaGetArgument_AudioStream(L, 1);
        void *arg2 = LuaGetArgument_ptr(L, 2);
        int arg3 = LuaGetArgument_int(L, 3);
        UpdateAudioStream(arg1, arg2, arg3);
        return 0;
    }

    // Close audio stream and free memory
    inline static int lua_CloseAudioStream(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        UnloadAudioStream(stream);
        return 0;
    }

    // Play audio stream
    inline static int lua_PlayAudioStream(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        PlayAudioStream(stream);
        return 0;
    }

    // Pause audio stream
    inline static int lua_PauseAudioStream(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        PauseAudioStream(stream);
        return 0;
    }

    // Resume audio stream
    inline static int lua_ResumeAudioStream(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        ResumeAudioStream(stream);
        return 0;
    }

    // Check if audio stream is playing
    inline static int lua_IsAudioStreamPlaying(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        bool result = IsAudioStreamPlaying(stream);
        LuaPush_bool(L, result);
        return 1;
    }

    // Stop audio stream
    inline static int lua_StopAudioStream(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        StopAudioStream(stream);
        return 0;
    }

    // Set volume for audio stream (1.0 is max level)
    inline static int lua_SetAudioStreamVolume(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        float volume = LuaGetArgument_float(L, 2);
        SetAudioStreamVolume(stream, volume);
        return 0;
    }

    // Set pitch for audio stream (1.0 is base level)
    inline static int lua_SetAudioStreamPitch(lua_State *L)
    {
        AudioStream stream = LuaGetArgument_AudioStream(L, 1);
        float pitch = LuaGetArgument_float(L, 2);
        SetAudioStreamPitch(stream, pitch);
        return 0;
    }

    // TODO: FROM HERE DOWN!

    //----------------------------------------------------------------------------------
    // Module Functions Definition - Utils math
    //----------------------------------------------------------------------------------
    inline static int lua_Clamp(lua_State *L)
    {
        float arg1 = LuaGetArgument_float(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        float arg3 = LuaGetArgument_float(L, 3);
        float result = Clamp(arg1, arg2, arg3);
        LuaPush_float(L, result);
        return 1;
    }

    //----------------------------------------------------------------------------------
    // Module Functions Definition - Vector2 math
    //----------------------------------------------------------------------------------
    inline static int lua_Vector2Zero(lua_State *L)
    {
        Vector2 result = Vector2Zero();
        LuaPush_Vector2(L, result);
        return 1;
    }

    inline static int lua_Vector2One(lua_State *L)
    {
        Vector2 result = Vector2One();
        LuaPush_Vector2(L, result);
        return 1;
    }

    inline static int lua_Vector2Add(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        Vector2 arg2 = LuaGetArgument_Vector2(L, 2);
        Vector2 result = Vector2Add(arg1, arg2);
        LuaPush_Vector2(L, result);
        return 1;
    }

    inline static int lua_Vector2Subtract(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        Vector2 arg2 = LuaGetArgument_Vector2(L, 2);
        Vector2 result = Vector2Subtract(arg1, arg2);
        LuaPush_Vector2(L, result);
        return 1;
    }

    inline static int lua_Vector2Lenght(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        float result = Vector2Length(arg1);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_Vector2DotProduct(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        Vector2 arg2 = LuaGetArgument_Vector2(L, 2);
        float result = Vector2DotProduct(arg1, arg2);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_Vector2Distance(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        Vector2 arg2 = LuaGetArgument_Vector2(L, 2);
        float result = Vector2Distance(arg1, arg2);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_Vector2Angle(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        Vector2 arg2 = LuaGetArgument_Vector2(L, 2);
        float result = Vector2Angle(arg1, arg2);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_Vector2Scale(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        Vector2 result = Vector2Scale(arg1, arg2);
        LuaPush_Vector2(L, result);
        return 1;
    }

    inline static int lua_Vector2Negate(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        Vector2Negate(arg1);
        LuaPush_Vector2(L, arg1);
        return 1;
    }

    inline static int lua_Vector2Divide(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        Vector2 result = Vector2Scale(arg1, 1.f / arg2);
        LuaPush_Vector2(L, result);
        return 1;
    }

    inline static int lua_Vector2Normalize(lua_State *L)
    {
        Vector2 arg1 = LuaGetArgument_Vector2(L, 1);
        Vector2Normalize(arg1);
        LuaPush_Vector2(L, arg1);
        return 1;
    }

    //----------------------------------------------------------------------------------
    // raylib [raymath] module functions - Vector3 math
    //----------------------------------------------------------------------------------
    inline static int lua_Vector3Zero(lua_State *L)
    {
        Vector3 result = Vector3Zero();
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3One(lua_State *L)
    {
        Vector3 result = Vector3One();
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3Add(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        Vector3 result = Vector3Add(arg1, arg2);
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3Subtract(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        Vector3 result = Vector3Subtract(arg1, arg2);
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3CrossProduct(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        Vector3 result = Vector3CrossProduct(arg1, arg2);
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3Perpendicular(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 result = Vector3Perpendicular(arg1);
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3DotProduct(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        float result = Vector3DotProduct(arg1, arg2);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_Vector3Length(lua_State *L)
    {
        const Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        float result = Vector3Length(arg1);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_Vector3Scale(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        Vector3Scale(arg1, arg2);
        LuaPush_Vector3(L, arg1);
        return 1;
    }

    inline static int lua_Vector3Negate(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3Negate(arg1);
        LuaPush_Vector3(L, arg1);
        return 1;
    }

    inline static int lua_Vector3Normalize(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3Normalize(arg1);
        LuaPush_Vector3(L, arg1);
        return 1;
    }

    inline static int lua_Vector3Distance(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        float result = Vector3Distance(arg1, arg2);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_Vector3Lerp(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        float arg3 = LuaGetArgument_float(L, 3);
        Vector3 result = Vector3Lerp(arg1, arg2, arg3);
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3Reflect(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        Vector3 result = Vector3Reflect(arg1, arg2);
        LuaPush_Vector3(L, result);
        return 1;
    }

    inline static int lua_Vector3Transform(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Matrix arg2 = LuaGetArgument_Matrix(L, 2);
        Vector3Transform(arg1, arg2);
        LuaPush_Vector3(L, arg1);
        return 1;
    }

    inline static int lua_Vector4Lerp(lua_State *L)
    {
        Vector4 arg1 = LuaGetArgument_Vector4(L, 1);
        Vector4 arg2 = LuaGetArgument_Vector4(L, 2);
        float arg3 = LuaGetArgument_float(L, 3);
        Vector4 result = Vector4Lerp(arg1, arg2, arg3);
        LuaPush_Vector4(L, result);
        return 1;
    }

    //----------------------------------------------------------------------------------
    // raylib [raymath] module functions - Matrix math
    //----------------------------------------------------------------------------------
    inline static int lua_MatrixDeterminant(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        float result = MatrixDeterminant(arg1);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_MatrixTrace(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        float result = MatrixTrace(arg1);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_MatrixTranspose(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        MatrixTranspose(arg1);
        LuaPush_Matrix(L, &arg1);
        return 1;
    }

    inline static int lua_MatrixInvert(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        MatrixInvert(arg1);
        LuaPush_Matrix(L, &arg1);
        return 1;
    }

    inline static int lua_MatrixIdentity(lua_State *L)
    {
        Matrix result = MatrixIdentity();
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixAdd(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        Matrix arg2 = LuaGetArgument_Matrix(L, 2);
        Matrix result = MatrixAdd(arg1, arg2);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixSubtract(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        Matrix arg2 = LuaGetArgument_Matrix(L, 2);
        Matrix result = MatrixSubtract(arg1, arg2);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixTranslate(lua_State *L)
    {
        float arg1 = LuaGetArgument_float(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        float arg3 = LuaGetArgument_float(L, 3);
        Matrix result = MatrixTranslate(arg1, arg2, arg3);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixRotate(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        Matrix result = MatrixRotate(arg1, arg2);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixRotateX(lua_State *L)
    {
        float arg1 = LuaGetArgument_float(L, 1);
        Matrix result = MatrixRotateX(arg1);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixRotateY(lua_State *L)
    {
        float arg1 = LuaGetArgument_float(L, 1);
        Matrix result = MatrixRotateY(arg1);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixRotateZ(lua_State *L)
    {
        float arg1 = LuaGetArgument_float(L, 1);
        Matrix result = MatrixRotateZ(arg1);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixScale(lua_State *L)
    {
        float arg1 = LuaGetArgument_float(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        float arg3 = LuaGetArgument_float(L, 3);
        Matrix result = MatrixScale(arg1, arg2, arg3);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixMultiply(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        Matrix arg2 = LuaGetArgument_Matrix(L, 2);
        Matrix result = MatrixMultiply(arg1, arg2);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixFrustum(lua_State *L)
    {
        double arg1 = LuaGetArgument_double(L, 1);
        double arg2 = LuaGetArgument_double(L, 2);
        double arg3 = LuaGetArgument_double(L, 3);
        double arg4 = LuaGetArgument_double(L, 4);
        double arg5 = LuaGetArgument_double(L, 5);
        double arg6 = LuaGetArgument_double(L, 6);
        Matrix result = MatrixFrustum(arg1, arg2, arg3, arg4, arg5, arg6);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixPerspective(lua_State *L)
    {
        double arg1 = LuaGetArgument_double(L, 1);
        double arg2 = LuaGetArgument_double(L, 2);
        double arg3 = LuaGetArgument_double(L, 3);
        double arg4 = LuaGetArgument_double(L, 4);
        Matrix result = MatrixPerspective(arg1, arg2, arg3, arg4);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixOrtho(lua_State *L)
    {
        double arg1 = LuaGetArgument_double(L, 1);
        double arg2 = LuaGetArgument_double(L, 2);
        double arg3 = LuaGetArgument_double(L, 3);
        double arg4 = LuaGetArgument_double(L, 4);
        double arg5 = LuaGetArgument_double(L, 5);
        double arg6 = LuaGetArgument_double(L, 6);
        Matrix result = MatrixOrtho(arg1, arg2, arg3, arg4, arg5, arg6);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_MatrixLookAt(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        Vector3 arg2 = LuaGetArgument_Vector3(L, 2);
        Vector3 arg3 = LuaGetArgument_Vector3(L, 3);
        Matrix result = MatrixLookAt(arg1, arg2, arg3);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    //----------------------------------------------------------------------------------
    // raylib [raymath] module functions - Quaternion math
    //----------------------------------------------------------------------------------
    inline static int lua_QuaternionLength(lua_State *L)
    {
        Quaternion arg1 = LuaGetArgument_Quaternion(L, 1);
        float result = QuaternionLength(arg1);
        LuaPush_float(L, result);
        return 1;
    }

    inline static int lua_QuaternionMultiply(lua_State *L)
    {
        Quaternion arg1 = LuaGetArgument_Quaternion(L, 1);
        Quaternion arg2 = LuaGetArgument_Quaternion(L, 2);
        Quaternion result = QuaternionMultiply(arg1, arg2);
        LuaPush_Quaternion(L, result);
        return 1;
    }

    inline static int lua_QuaternionSlerp(lua_State *L)
    {
        Quaternion arg1 = LuaGetArgument_Quaternion(L, 1);
        Quaternion arg2 = LuaGetArgument_Quaternion(L, 2);
        float arg3 = LuaGetArgument_float(L, 3);
        Quaternion result = QuaternionSlerp(arg1, arg2, arg3);
        LuaPush_Quaternion(L, result);
        return 1;
    }

    inline static int lua_QuaternionFromMatrix(lua_State *L)
    {
        Matrix arg1 = LuaGetArgument_Matrix(L, 1);
        Quaternion result = QuaternionFromMatrix(arg1);
        LuaPush_Quaternion(L, result);
        return 1;
    }

    inline static int lua_QuaternionToMatrix(lua_State *L)
    {
        Quaternion arg1 = LuaGetArgument_Quaternion(L, 1);
        Matrix result = QuaternionToMatrix(arg1);
        LuaPush_Matrix(L, &result);
        return 1;
    }

    inline static int lua_QuaternionFromAxisAngle(lua_State *L)
    {
        Vector3 arg1 = LuaGetArgument_Vector3(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        Quaternion result = QuaternionFromAxisAngle(arg1, arg2);
        LuaPush_Quaternion(L, result);
        return 1;
    }

    inline static int lua_QuaternionToAxisAngle(lua_State *L)
    {
        Quaternion arg1 = LuaGetArgument_Quaternion(L, 1);
        Vector3 arg2;
        float arg3 = 0;
        QuaternionToAxisAngle(arg1, &arg2, &arg3);
        LuaPush_Vector3(L, arg2);
        LuaPush_float(L, arg3);
        return 2;
    }

    inline static int lua_QuaternionFromEuler(lua_State *L)
    {
        float arg1 = LuaGetArgument_float(L, 1);
        float arg2 = LuaGetArgument_float(L, 2);
        float arg3 = LuaGetArgument_float(L, 3);
        Quaternion result = QuaternionFromEuler(arg1, arg2, arg3);
        LuaPush_Quaternion(L, result);
        return 1;
    }

    inline static int lua_QuaternionToEuler(lua_State *L)
    {
        Quaternion arg1 = LuaGetArgument_Quaternion(L, 1);
        Vector3 result = QuaternionToEuler(arg1);
        LuaPush_Vector3(L, result);
        return 1;
    }

    //----------------------------------------------------------------------------------
    // Functions Registering
    //----------------------------------------------------------------------------------

#ifdef RAYLIB_LUA_IMPLEMENTATION
#define REG(name) {#name, lua_##name}
    luaL_Reg raylib_functions[] = {

        // Register non-opaque data types
        REG(Color),
        REG(Vector2),
        REG(Vector3),
        REG(Vector4),
        // REG(Matrix)
        REG(Quaternion),
        REG(Rectangle),
        REG(Ray),
        REG(RayHitInfo),
        REG(Camera),
        REG(Camera2D),
        REG(BoundingBox),
        // REG(Material)

        REG(ColorAlpha),
        REG(ColorBrightness),
        REG(ColorFromNormalized),

        // Register functions
        //--------------------
        REG(InitWindow),
        REG(CloseWindow),
        REG(IsWindowReady),
        REG(WindowShouldClose),
        REG(IsWindowMinimized),
        REG(ToggleFullscreen),
        REG(SetWindowIcon),
        REG(SetWindowTitle),
        REG(SetWindowPosition),
        REG(SetWindowMonitor),
        REG(SetWindowMinSize),
        REG(SetWindowSize),
        REG(GetScreenWidth),
        REG(GetScreenHeight),
        REG(ShowCursor),
        REG(HideCursor),
        REG(IsCursorHidden),
        REG(EnableCursor),
        REG(DisableCursor),
        REG(ClearBackground),
        REG(BeginDrawing),
        REG(EndDrawing),
        REG(BeginMode2D),
        REG(EndMode2D),
        REG(BeginMode3D),
        REG(EndMode3D),
        REG(BeginTextureMode),
        REG(EndTextureMode),
        REG(GetMouseRay),
        REG(GetWorldToScreen),
        REG(GetCameraMatrix),
        REG(SetTargetFPS),
        REG(GetFPS),
        REG(GetFrameTime),
        REG(GetTime),
        REG(ColorToInt),
        REG(ColorNormalize),
        REG(ColorToHSV),
        REG(ColorLerp),
        REG(GetColor),
        REG(Fade),
        REG(SetConfigFlags),
        REG(GetRandomValue),
        REG(IsKeyPressed),
        REG(IsKeyDown),
        REG(IsKeyReleased),
        REG(IsKeyUp),
        REG(GetKeyPressed),
        REG(GetCharPressed),
        REG(SetExitKey),
        REG(IsMouseButtonPressed),
        REG(IsMouseButtonDown),
        REG(IsMouseButtonReleased),
        REG(IsMouseButtonUp),
        REG(GetMouseX),
        REG(GetMouseY),
        REG(GetMousePosition),
        REG(SetMousePosition),
        REG(SetMouseScale),
        REG(GetMouseWheelMove),
        REG(GetTouchX),
        REG(GetTouchY),
        REG(GetTouchPosition),
        REG(SetGesturesEnabled),
        REG(IsGestureDetected),
        REG(GetGestureDetected),
        REG(GetTouchPointsCount),
        REG(GetGestureHoldDuration),
        REG(GetGestureDragVector),
        REG(GetGestureDragAngle),
        REG(GetGesturePinchVector),
        REG(GetGesturePinchAngle),
        REG(SetCameraMode),
        REG(UpdateCamera),
        REG(SetCameraPanControl),
        REG(SetCameraAltControl),
        REG(SetCameraSmoothZoomControl),
        REG(SetCameraMoveControls),
        REG(DrawPixel),
        REG(DrawPixelV),
        REG(DrawLine),
        REG(DrawLineV),
        REG(DrawLineEx),
        REG(DrawLineBezier),
        REG(DrawCircle),
        REG(DrawCircleGradient),
        REG(DrawCircleV),
        REG(DrawCircleLines),
        REG(DrawRectangle),
        REG(DrawRectangleV),
        REG(DrawRectangleRec),
        REG(DrawRectanglePro),
        REG(DrawRectangleGradientV),
        REG(DrawRectangleGradientH),
        REG(DrawRectangleGradientEx),
        REG(DrawRectangleLines),
        REG(DrawRectangleLinesEx),
        REG(DrawRectangleRounded),
        REG(DrawRectangleRoundedLines),
        REG(DrawRectangleRoundedLinesEx),
        REG(DrawTriangle),
        REG(DrawTriangleLines),
        REG(DrawPoly),
        REG(CheckCollisionRecs),
        REG(CheckCollisionCircles),
        REG(CheckCollisionCircleRec),
        REG(GetCollisionRec),
        REG(CheckCollisionPointRec),
        REG(CheckCollisionPointCircle),
        REG(CheckCollisionPointTriangle),
        REG(LoadTexture),
        REG(LoadTextureFromImage),
        REG(LoadRenderTexture),
        REG(UnloadImage),
        REG(UnloadTexture),
        REG(UnloadRenderTexture),
        REG(UpdateTexture),
        REG(DrawTexture),
        REG(DrawTextureV),
        REG(DrawTextureEx),
        REG(DrawTextureRec),
        REG(DrawTexturePro),
        REG(GetFontDefault),
        REG(LoadFont),
        REG(LoadFontEx),
        REG(UnloadFont),
        REG(DrawFPS),
        REG(DrawText),
        REG(DrawTextEx),
        REG(MeasureText),
        REG(MeasureTextEx),
        REG(GetGlyphIndex),
        REG(DrawLine3D),
        REG(DrawCircle3D),
        REG(DrawCube),
        REG(DrawCubeV),
        REG(DrawCubeWires),
        REG(DrawSphere),
        REG(DrawSphereEx),
        REG(DrawSphereWires),
        REG(DrawCylinder),
        REG(DrawCylinderWires),
        REG(DrawPlane),
        REG(DrawRay),
        REG(DrawBoundingBox),
        REG(DrawBillboard),
        REG(LoadShader),
        REG(LoadShaderFromMemory),
        REG(UnloadShader),
        REG(GetShaderLocation),
        REG(BeginShaderMode),
        REG(EndShaderMode),
        REG(BeginBlendMode),
        REG(EndBlendMode),
        REG(InitAudioDevice),
        REG(CloseAudioDevice),
        REG(IsAudioDeviceReady),
        REG(SetMasterVolume),
        REG(LoadWave),
        REG(LoadSound),
        REG(LoadSoundFromWave),
        REG(UpdateSound),
        REG(UnloadWave),
        REG(UnloadSound),
        REG(PlaySound),
        REG(PauseSound),
        REG(ResumeSound),
        REG(StopSound),
        REG(IsSoundPlaying),
        REG(SetSoundVolume),
        REG(SetSoundPitch),
        REG(LoadMusicStream),
        REG(UnloadMusicStream),
        REG(PlayMusicStream),
        REG(UpdateMusicStream),
        REG(StopMusicStream),
        REG(PauseMusicStream),
        REG(ResumeMusicStream),
        REG(IsMusicPlaying),
        REG(SetMusicVolume),
        REG(SetMusicPitch),
        REG(GetMusicTimeLength),
        REG(GetMusicTimePlayed),
        REG(InitAudioStream),
        REG(UpdateAudioStream),
        REG(CloseAudioStream),
        REG(PlayAudioStream),
        REG(PauseAudioStream),
        REG(ResumeAudioStream),
        REG(IsAudioStreamPlaying),
        REG(StopAudioStream),
        REG(SetAudioStreamVolume),
        REG(SetAudioStreamPitch),

        // [raymath] module functions - general
        REG(Clamp),

        // [raymath] module functions - Vector2
        REG(Vector2Zero),
        REG(Vector2One),
        REG(Vector2Add),
        REG(Vector2Subtract),
        REG(Vector2Lenght),
        REG(Vector2DotProduct),
        REG(Vector2Distance),
        REG(Vector2Angle),
        REG(Vector2Scale),
        REG(Vector2Negate),
        REG(Vector2Divide),
        REG(Vector2Normalize),

        // [raymath] module functions - Vector3
        REG(Vector3Zero),
        REG(Vector3One),
        REG(Vector3Add),
        REG(Vector3Subtract),
        REG(Vector3CrossProduct),
        REG(Vector3Perpendicular),
        REG(Vector3Length),
        REG(Vector3DotProduct),
        REG(Vector3Distance),
        REG(Vector3Scale),
        REG(Vector3Negate),
        REG(Vector3Normalize),
        REG(Vector3Transform),
        REG(Vector3Lerp),
        REG(Vector3Reflect),

        // [raymath] module functions - Vector4
        REG(Vector4Lerp),

        // [raymath] module functions - Matrix
        REG(MatrixDeterminant),
        REG(MatrixTrace),
        REG(MatrixTranspose),
        REG(MatrixInvert),
        REG(MatrixIdentity),
        REG(MatrixAdd),
        REG(MatrixTranslate),
        REG(MatrixRotate),
        REG(MatrixRotateX),
        REG(MatrixRotateY),
        REG(MatrixRotateZ),
        REG(MatrixScale),
        REG(MatrixMultiply),
        REG(MatrixFrustum),
        REG(MatrixPerspective),
        REG(MatrixOrtho),
        REG(MatrixLookAt),

        // [raymath] module functions - Quaternion
        REG(QuaternionLength),
        REG(QuaternionMultiply),
        REG(QuaternionSlerp),
        REG(QuaternionFromMatrix),
        REG(QuaternionToMatrix),
        REG(QuaternionFromAxisAngle),
        REG(QuaternionToAxisAngle),
        REG(QuaternionFromEuler),
        REG(QuaternionToEuler),

        {NULL, NULL} // sentinel: end signal
    };
#undef REG
#else
extern luaL_Reg raylib_functions[];
#endif

#ifdef RAYLIB_LUA_IMPLEMENTATION
    // Register raylib Lua functionality
    void rLuaRegisterFunctions(const char *opt_table)
    {
        if (opt_table)
            lua_createtable(L, 0, sizeof(raylib_functions) / sizeof(raylib_functions[0]));
        else
            lua_pushglobaltable(L);

        luaL_setfuncs(L, raylib_functions, 0);
    }

    // Initialize Lua system
    void rLuaInitDeviceFromExisting(lua_State *lua)
    {
        mainLuaState = lua;
        L = lua;

        LuaStartEnum();
        LuaSetEnum("SHOW_LOGO", 1);
        LuaSetEnum("FULLSCREEN_MODE", 2);
        LuaSetEnum("WINDOW_RESIZABLE", 4);
        LuaSetEnum("WINDOW_DECORATED", 8);
        LuaSetEnum("WINDOW_TRANSPARENT", 16);
        LuaSetEnum("MSAA_4X_HINT", 32);
        LuaSetEnum("VSYNC_HINT", 64);
        LuaEndEnum("FLAG");

        LuaStartEnum();
        LuaSetEnum("SPACE", 32);
        LuaSetEnum("ESCAPE", 256);
        LuaSetEnum("ENTER", 257);
        LuaSetEnum("BACKSPACE", 259);
        LuaSetEnum("RIGHT", 262);
        LuaSetEnum("LEFT", 263);
        LuaSetEnum("DOWN", 264);
        LuaSetEnum("UP", 265);
        LuaSetEnum("F1", 290);
        LuaSetEnum("F2", 291);
        LuaSetEnum("F3", 292);
        LuaSetEnum("F4", 293);
        LuaSetEnum("F5", 294);
        LuaSetEnum("F6", 295);
        LuaSetEnum("F7", 296);
        LuaSetEnum("F8", 297);
        LuaSetEnum("F9", 298);
        LuaSetEnum("F10", 299);
        LuaSetEnum("LEFT_SHIFT", 340);
        LuaSetEnum("LEFT_CONTROL", 341);
        LuaSetEnum("LEFT_ALT", 342);
        LuaSetEnum("RIGHT_SHIFT", 344);
        LuaSetEnum("RIGHT_CONTROL", 345);
        LuaSetEnum("RIGHT_ALT", 346);
        LuaSetEnum("ZERO", 48);
        LuaSetEnum("ONE", 49);
        LuaSetEnum("TWO", 50);
        LuaSetEnum("THREE", 51);
        LuaSetEnum("FOUR", 52);
        LuaSetEnum("FIVE", 53);
        LuaSetEnum("SIX", 54);
        LuaSetEnum("SEVEN", 55);
        LuaSetEnum("EIGHT", 56);
        LuaSetEnum("NINE", 57);
        LuaSetEnum("A", 65);
        LuaSetEnum("B", 66);
        LuaSetEnum("C", 67);
        LuaSetEnum("D", 68);
        LuaSetEnum("E", 69);
        LuaSetEnum("F", 70);
        LuaSetEnum("G", 71);
        LuaSetEnum("H", 72);
        LuaSetEnum("I", 73);
        LuaSetEnum("J", 74);
        LuaSetEnum("K", 75);
        LuaSetEnum("L", 76);
        LuaSetEnum("M", 77);
        LuaSetEnum("N", 78);
        LuaSetEnum("O", 79);
        LuaSetEnum("P", 80);
        LuaSetEnum("Q", 81);
        LuaSetEnum("R", 82);
        LuaSetEnum("S", 83);
        LuaSetEnum("T", 84);
        LuaSetEnum("U", 85);
        LuaSetEnum("V", 86);
        LuaSetEnum("W", 87);
        LuaSetEnum("X", 88);
        LuaSetEnum("Y", 89);
        LuaSetEnum("Z", 90);
        LuaEndEnum("KEY");

        LuaStartEnum();
        LuaSetEnum("LEFT_BUTTON", 0);
        LuaSetEnum("RIGHT_BUTTON", 1);
        LuaSetEnum("MIDDLE_BUTTON", 2);
        LuaEndEnum("MOUSE");

        LuaStartEnum();
        LuaSetEnum("PLAYER1", 0);
        LuaSetEnum("PLAYER2", 1);
        LuaSetEnum("PLAYER3", 2);
        LuaSetEnum("PLAYER4", 3);

        LuaSetEnum("PS3_BUTTON_TRIANGLE", 0);
        LuaSetEnum("PS3_BUTTON_CIRCLE", 1);
        LuaSetEnum("PS3_BUTTON_CROSS", 2);
        LuaSetEnum("PS3_BUTTON_SQUARE", 3);
        LuaSetEnum("PS3_BUTTON_L1", 6);
        LuaSetEnum("PS3_BUTTON_R1", 7);
        LuaSetEnum("PS3_BUTTON_L2", 4);
        LuaSetEnum("PS3_BUTTON_R2", 5);
        LuaSetEnum("PS3_BUTTON_START", 8);
        LuaSetEnum("PS3_BUTTON_SELECT", 9);
        LuaSetEnum("PS3_BUTTON_UP", 24);
        LuaSetEnum("PS3_BUTTON_RIGHT", 25);
        LuaSetEnum("PS3_BUTTON_DOWN", 26);
        LuaSetEnum("PS3_BUTTON_LEFT", 27);
        LuaSetEnum("PS3_BUTTON_PS", 12);
        LuaSetEnum("PS3_AXIS_LEFT_X", 0);
        LuaSetEnum("PS3_AXIS_LEFT_Y", 1);
        LuaSetEnum("PS3_AXIS_RIGHT_X", 2);
        LuaSetEnum("PS3_AXIS_RIGHT_Y", 5);
        LuaSetEnum("PS3_AXIS_L2", 3); // [1..-1] (pressure-level)
        LuaSetEnum("PS3_AXIS_R2", 4); // [1..-1] (pressure-level)

        // Xbox360 USB Controller Buttons
        LuaSetEnum("XBOX_BUTTON_A", 0);
        LuaSetEnum("XBOX_BUTTON_B", 1);
        LuaSetEnum("XBOX_BUTTON_X", 2);
        LuaSetEnum("XBOX_BUTTON_Y", 3);
        LuaSetEnum("XBOX_BUTTON_LB", 4);
        LuaSetEnum("XBOX_BUTTON_RB", 5);
        LuaSetEnum("XBOX_BUTTON_SELECT", 6);
        LuaSetEnum("XBOX_BUTTON_START", 7);
        LuaSetEnum("XBOX_BUTTON_UP", 10);
        LuaSetEnum("XBOX_BUTTON_RIGHT", 11);
        LuaSetEnum("XBOX_BUTTON_DOWN", 12);
        LuaSetEnum("XBOX_BUTTON_LEFT", 13);
        LuaSetEnum("XBOX_BUTTON_HOME", 8);
#if defined(PLATFORM_RPI)
        LuaSetEnum("XBOX_AXIS_LEFT_X", 0);  // [-1..1] (left->right)
        LuaSetEnum("XBOX_AXIS_LEFT_Y", 1);  // [-1..1] (up->down)
        LuaSetEnum("XBOX_AXIS_RIGHT_X", 3); // [-1..1] (left->right)
        LuaSetEnum("XBOX_AXIS_RIGHT_Y", 4); // [-1..1] (up->down)
        LuaSetEnum("XBOX_AXIS_LT", 2);      // [-1..1] (pressure-level)
        LuaSetEnum("XBOX_AXIS_RT", 5);      // [-1..1] (pressure-level)
#else
        LuaSetEnum("XBOX_AXIS_LEFT_X", 0);  // [-1..1] (left->right)
        LuaSetEnum("XBOX_AXIS_LEFT_Y", 1);  // [1..-1] (up->down)
        LuaSetEnum("XBOX_AXIS_RIGHT_X", 2); // [-1..1] (left->right)
        LuaSetEnum("XBOX_AXIS_RIGHT_Y", 3); // [1..-1] (up->down)
        LuaSetEnum("XBOX_AXIS_LT", 4);      // [-1..1] (pressure-level)
        LuaSetEnum("XBOX_AXIS_RT", 5);      // [-1..1] (pressure-level)
#endif
        LuaEndEnum("GAMEPAD");

        lua_pushglobaltable(L);
        LuaSetEnumColor("LIGHTGRAY", LIGHTGRAY);
        LuaSetEnumColor("GRAY", GRAY);
        LuaSetEnumColor("DARKGRAY", DARKGRAY);
        LuaSetEnumColor("YELLOW", YELLOW);
        LuaSetEnumColor("GOLD", GOLD);
        LuaSetEnumColor("ORANGE", ORANGE);
        LuaSetEnumColor("PINK", PINK);
        LuaSetEnumColor("RED", RED);
        LuaSetEnumColor("MAROON", MAROON);
        LuaSetEnumColor("GREEN", GREEN);
        LuaSetEnumColor("LIME", LIME);
        LuaSetEnumColor("DARKGREEN", DARKGREEN);
        LuaSetEnumColor("SKYBLUE", SKYBLUE);
        LuaSetEnumColor("BLUE", BLUE);
        LuaSetEnumColor("DARKBLUE", DARKBLUE);
        LuaSetEnumColor("PURPLE", PURPLE);
        LuaSetEnumColor("VIOLET", VIOLET);
        LuaSetEnumColor("DARKPURPLE", DARKPURPLE);
        LuaSetEnumColor("BEIGE", BEIGE);
        LuaSetEnumColor("BROWN", BROWN);
        LuaSetEnumColor("DARKBROWN", DARKBROWN);
        LuaSetEnumColor("WHITE", WHITE);
        LuaSetEnumColor("BLACK", BLACK);
        LuaSetEnumColor("BLANK", BLANK);
        LuaSetEnumColor("MAGENTA", MAGENTA);
        LuaSetEnumColor("RAYWHITE", RAYWHITE);
        lua_pop(L, 1);

        /*
        LuaStartEnum();
        LuaSetEnum("UNCOMPRESSED_GRAYSCALE", UNCOMPRESSED_GRAYSCALE);
        LuaSetEnum("UNCOMPRESSED_GRAY_ALPHA", UNCOMPRESSED_GRAY_ALPHA);
        LuaSetEnum("UNCOMPRESSED_R5G6B5", UNCOMPRESSED_R5G6B5);
        LuaSetEnum("UNCOMPRESSED_R8G8B8", UNCOMPRESSED_R8G8B8);
        LuaSetEnum("UNCOMPRESSED_R5G5B5A1", UNCOMPRESSED_R5G5B5A1);
        LuaSetEnum("UNCOMPRESSED_R4G4B4A4", UNCOMPRESSED_R4G4B4A4);
        LuaSetEnum("UNCOMPRESSED_R8G8B8A8", UNCOMPRESSED_R8G8B8A8);
        LuaSetEnum("COMPRESSED_DXT1_RGB", COMPRESSED_DXT1_RGB);
        LuaSetEnum("COMPRESSED_DXT1_RGBA", COMPRESSED_DXT1_RGBA);
        LuaSetEnum("COMPRESSED_DXT3_RGBA", COMPRESSED_DXT3_RGBA);
        LuaSetEnum("COMPRESSED_DXT5_RGBA", COMPRESSED_DXT5_RGBA);
        LuaSetEnum("COMPRESSED_ETC1_RGB", COMPRESSED_ETC1_RGB);
        LuaSetEnum("COMPRESSED_ETC2_RGB", COMPRESSED_ETC2_RGB);
        LuaSetEnum("COMPRESSED_ETC2_EAC_RGBA", COMPRESSED_ETC2_EAC_RGBA);
        LuaSetEnum("COMPRESSED_PVRT_RGB", COMPRESSED_PVRT_RGB);
        LuaSetEnum("COMPRESSED_PVRT_RGBA", COMPRESSED_PVRT_RGBA);
        LuaSetEnum("COMPRESSED_ASTC_4x4_RGBA", COMPRESSED_ASTC_4x4_RGBA);
        LuaSetEnum("COMPRESSED_ASTC_8x8_RGBA", COMPRESSED_ASTC_8x8_RGBA);
        LuaEndEnum("TextureFormat");
      */

        LuaStartEnum();
        LuaSetEnum("ALPHA", BLEND_ALPHA);
        LuaSetEnum("ADDITIVE", BLEND_ADDITIVE);
        LuaSetEnum("MULTIPLIED", BLEND_MULTIPLIED);
        LuaEndEnum("BlendMode");

        /*
        LuaStartEnum();
        LuaSetEnum("POINT", FILTER_POINT);
        LuaSetEnum("BILINEAR", FILTER_BILINEAR);
        LuaSetEnum("TRILINEAR", FILTER_TRILINEAR);
        LuaSetEnum("ANISOTROPIC_4X", FILTER_ANISOTROPIC_4X);
        LuaSetEnum("ANISOTROPIC_8X", FILTER_ANISOTROPIC_8X);
        LuaSetEnum("ANISOTROPIC_16X", FILTER_ANISOTROPIC_16X);
        LuaEndEnum("TextureFilter");
      */

        LuaStartEnum();
        LuaSetEnum("NONE", GESTURE_NONE);
        LuaSetEnum("TAP", GESTURE_TAP);
        LuaSetEnum("DOUBLETAP", GESTURE_DOUBLETAP);
        LuaSetEnum("HOLD", GESTURE_HOLD);
        LuaSetEnum("DRAG", GESTURE_DRAG);
        LuaSetEnum("SWIPE_RIGHT", GESTURE_SWIPE_RIGHT);
        LuaSetEnum("SWIPE_LEFT", GESTURE_SWIPE_LEFT);
        LuaSetEnum("SWIPE_UP", GESTURE_SWIPE_UP);
        LuaSetEnum("SWIPE_DOWN", GESTURE_SWIPE_DOWN);
        LuaSetEnum("PINCH_IN", GESTURE_PINCH_IN);
        LuaSetEnum("PINCH_OUT", GESTURE_PINCH_OUT);
        LuaEndEnum("Gestures");

        LuaStartEnum();
        LuaSetEnum("CUSTOM", CAMERA_CUSTOM);
        LuaSetEnum("FREE", CAMERA_FREE);
        LuaSetEnum("ORBITAL", CAMERA_ORBITAL);
        LuaSetEnum("FIRST_PERSON", CAMERA_FIRST_PERSON);
        LuaSetEnum("THIRD_PERSON", CAMERA_THIRD_PERSON);
        LuaEndEnum("CameraMode");

        /*
        LuaStartEnum();
        LuaSetEnum("DEFAULT_DEVICE", HMD_DEFAULT_DEVICE);
        LuaSetEnum("OCULUS_RIFT_DK2", HMD_OCULUS_RIFT_DK2);
        LuaSetEnum("OCULUS_RIFT_CV1", HMD_OCULUS_RIFT_CV1);
        LuaSetEnum("VALVE_HTC_VIVE", HMD_VALVE_HTC_VIVE);
        LuaSetEnum("SAMSUNG_GEAR_VR", HMD_SAMSUNG_GEAR_VR);
        LuaSetEnum("GOOGLE_CARDBOARD", HMD_GOOGLE_CARDBOARD);
        LuaSetEnum("SONY_PLAYSTATION_VR", HMD_SONY_PLAYSTATION_VR);
        LuaSetEnum("RAZER_OSVR", HMD_RAZER_OSVR);
        LuaSetEnum("FOVE_VR", HMD_FOVE_VR);
        LuaEndEnum("VrDevice");
        lua_pushglobaltable(L);
        LuaSetEnum("INFO", INFO);
        LuaSetEnum("ERROR", ERROR);
        LuaSetEnum("WARNING", WARNING);
        LuaSetEnum("DEBUG", DEBUG);
        LuaSetEnum("OTHER", OTHER);
        lua_pop(L, 1);
      */

        LuaPush_bool(L, true);
#if defined(PLATFORM_DESKTOP)
        lua_setglobal(L, "PLATFORM_DESKTOP");
#elif defined(PLATFORM_ANDROID)
        lua_setglobal(L, "PLATFORM_ANDROID");
#elif defined(PLATFORM_RPI)
        lua_setglobal(L, "PLATFORM_RPI");
#elif defined(PLATFORM_WEB)
        lua_setglobal(L, "PLATFORM_WEB");
#endif

        LuaBuildOpaqueMetatables();

        rLuaRegisterFunctions(0); // Register Lua raylib functions
    }
#else
void rLuaRegisterFunctions(const char *opt_table);
void rLuaInitDeviceFromExisting(lua_State *lua);
#endif
#ifdef __cplusplus
}
#endif
