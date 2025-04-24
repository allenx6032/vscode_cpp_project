
#include "lua_CCCrypto.h"

#include "cocos2d.h"
#include "cocos2d-CCLuaEngine.h"

#include "md5.h"
#include "xxtea.h"

USING_NS_CC;

// xxtea

static int lxxtea_encrypt(lua_State* tolua_S)
{
    unsigned char * plaintext = ((unsigned char *)  tolua_tostring(tolua_S,1,0));
    xxtea_long plaintextLength = (xxtea_long)  lua_strlen(tolua_S, 1);
    unsigned char* key = ((unsigned char*)  tolua_tostring(tolua_S,2,0));
    xxtea_long keyLength = (xxtea_long)  lua_strlen(tolua_S, 2);
    xxtea_long resultLength = 0;
    unsigned char* result = xxtea_encrypt(plaintext, plaintextLength, key, keyLength, &resultLength);
    if (resultLength <= 0)
    {
        lua_pushnil(tolua_S);
    }
    else
    {
        lua_pushlstring(tolua_S, (const char*)result, resultLength);
        free(result);
    }
    
    return 1;
}

static int lxxtea_decrypt(lua_State* tolua_S)
{
    unsigned char * plaintext = ((unsigned char *)  tolua_tostring(tolua_S,1,0));
    xxtea_long plaintextLength = (xxtea_long)  lua_strlen(tolua_S, 1);
    unsigned char* key = ((unsigned char*)  tolua_tostring(tolua_S,2,0));
    xxtea_long keyLength = (xxtea_long)  lua_strlen(tolua_S, 2);
    xxtea_long resultLength = 0;
    unsigned char* result = xxtea_decrypt(plaintext, plaintextLength, key, keyLength, &resultLength);
    if (resultLength <= 0)
    {
        lua_pushnil(tolua_S);
    }
    else
    {
        lua_pushlstring(tolua_S, (const char*)result, resultLength);
        free(result);
    }
    
    return 1;
}

static const struct luaL_reg xxteaLib[] =
{
    { "encrypt", lxxtea_encrypt },
    { "decrypt", lxxtea_decrypt },
    { NULL, NULL }
};

int luaopen_xxtea(lua_State *L)
{
    luaL_openlib(L, "xxtea1", xxteaLib, 0);
    return 1;
}

// md5

static int lmd5_sum(lua_State* tolua_S)
{
    unsigned char* text = (unsigned char*)luaL_checkstring(tolua_S, 1);
    cocos2d::Data data;
    data.fastSet(text, lua_strlen(tolua_S,1));
    auto ret = cocos2d::utils::getDataMD5Hash(data);
    data.fastSet(0,0);
    
    lua_pushstring(tolua_S, ret.c_str());
    
    return 1;
}

static int lmd5_file(lua_State *tolua_S)
{
    const char* filename = luaL_checkstring(tolua_S, 1);
    auto ret = cocos2d::utils::getFileMD5Hash(filename);
    lua_pushstring(tolua_S, ret.c_str());
    return 1;
}

static const struct luaL_reg md5Lib[] =
{
    { "sum", lmd5_sum },
    { "file", lmd5_file },
    { NULL, NULL }
};

int luaopen_md5(lua_State *L)
{
    luaL_openlib(L, "md51", md5Lib, 0);
    return 1;
}
