#include "cocos2d-lua_cocos2dx_extension_auto.hpp"
#include "cocos-ext.h"
#include "cocos2d-tolua_fix.h"
#include "cocos2d-LuaBasicConversions.h"

int lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx'", nullptr);
            return 0;
        }
        cocos2d::extension::AssetsManagerEx* ret = cobj->getAssetsManagerEx();
        object_to_luaval<cocos2d::extension::AssetsManagerEx>(tolua_S, "cc.AssetsManagerEx",(cocos2d::extension::AssetsManagerEx*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getAssetsManagerEx",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId'", nullptr);
            return 0;
        }
        std::string ret = cobj->getAssetId();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getAssetId",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode'", nullptr);
            return 0;
        }
        int ret = cobj->getCURLECode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getCURLECode",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getMessage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getMessage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getMessage'", nullptr);
            return 0;
        }
        std::string ret = cobj->getMessage();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getMessage",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getMessage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode'", nullptr);
            return 0;
        }
        int ret = cobj->getCURLMCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getCURLMCode",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile'", nullptr);
            return 0;
        }
        double ret = cobj->getPercentByFile();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getPercentByFile",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getEventCode();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getEventCode",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_getPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercent'", nullptr);
            return 0;
        }
        double ret = cobj->getPercent();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:getPercent",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_getPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventAssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 4) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 5) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 6) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 7) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;
        std::string arg6;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 8,&arg6, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 8) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;
        std::string arg6;
        int arg7;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 8,&arg6, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 9,(int *)&arg7, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    if (argc == 9) 
    {
        std::string arg0;
        cocos2d::extension::AssetsManagerEx* arg1;
        cocos2d::extension::EventAssetsManagerEx::EventCode arg2;
        double arg3;
        double arg4;
        std::string arg5;
        std::string arg6;
        int arg7;
        int arg8;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_object<cocos2d::extension::AssetsManagerEx>(tolua_S, 3, "cc.AssetsManagerEx",&arg1, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 5,&arg3, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_number(tolua_S, 6,&arg4, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 7,&arg5, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 8,&arg6, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 9,(int *)&arg7, "cc.EventAssetsManagerEx:EventAssetsManagerEx");

        ok &= luaval_to_int32(tolua_S, 10,(int *)&arg8, "cc.EventAssetsManagerEx:EventAssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventAssetsManagerEx(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventAssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventAssetsManagerEx:EventAssetsManagerEx",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventAssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_EventAssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventAssetsManagerEx)");
    return 0;
}

int lua_register_cocos2dx_extension_EventAssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.EventAssetsManagerEx");
    tolua_cclass(tolua_S,"EventAssetsManagerEx","cc.EventAssetsManagerEx","cc.EventCustom",nullptr);

    tolua_beginmodule(tolua_S,"EventAssetsManagerEx");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_EventAssetsManagerEx_constructor);
        tolua_function(tolua_S,"getAssetsManagerEx",lua_cocos2dx_extension_EventAssetsManagerEx_getAssetsManagerEx);
        tolua_function(tolua_S,"getAssetId",lua_cocos2dx_extension_EventAssetsManagerEx_getAssetId);
        tolua_function(tolua_S,"getCURLECode",lua_cocos2dx_extension_EventAssetsManagerEx_getCURLECode);
        tolua_function(tolua_S,"getMessage",lua_cocos2dx_extension_EventAssetsManagerEx_getMessage);
        tolua_function(tolua_S,"getCURLMCode",lua_cocos2dx_extension_EventAssetsManagerEx_getCURLMCode);
        tolua_function(tolua_S,"getPercentByFile",lua_cocos2dx_extension_EventAssetsManagerEx_getPercentByFile);
        tolua_function(tolua_S,"getEventCode",lua_cocos2dx_extension_EventAssetsManagerEx_getEventCode);
        tolua_function(tolua_S,"getPercent",lua_cocos2dx_extension_EventAssetsManagerEx_getPercent);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::EventAssetsManagerEx).name();
    g_luaType[typeName] = "cc.EventAssetsManagerEx";
    g_typeCast["EventAssetsManagerEx"] = "cc.EventAssetsManagerEx";
    return 1;
}

int lua_cocos2dx_extension_Manifest_getManifestFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getManifestFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getManifestFileUrl'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getManifestFileUrl();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getManifestFileUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getManifestFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_isVersionLoaded(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_isVersionLoaded'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_isVersionLoaded'", nullptr);
            return 0;
        }
        bool ret = cobj->isVersionLoaded();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:isVersionLoaded",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_isVersionLoaded'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_isLoaded(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_isLoaded'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_isLoaded'", nullptr);
            return 0;
        }
        bool ret = cobj->isLoaded();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:isLoaded",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_isLoaded'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getPackageUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getPackageUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getPackageUrl'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getPackageUrl();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getPackageUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getPackageUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getVersion(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getVersion'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getVersion'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getVersion();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getVersion",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getVersion'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getVersionFileUrl(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getVersionFileUrl'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getVersionFileUrl'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getVersionFileUrl();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getVersionFileUrl",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getVersionFileUrl'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_Manifest_getSearchPaths(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::Manifest* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.Manifest",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::Manifest*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_Manifest_getSearchPaths'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_Manifest_getSearchPaths'", nullptr);
            return 0;
        }
        std::vector<std::string> ret = cobj->getSearchPaths();
        ccvector_std_string_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.Manifest:getSearchPaths",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_Manifest_getSearchPaths'.",&tolua_err);
#endif

    return 0;
}
static int lua_cocos2dx_extension_Manifest_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Manifest)");
    return 0;
}

int lua_register_cocos2dx_extension_Manifest(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.Manifest");
    tolua_cclass(tolua_S,"Manifest","cc.Manifest","cc.Ref",nullptr);

    tolua_beginmodule(tolua_S,"Manifest");
        tolua_function(tolua_S,"getManifestFileUrl",lua_cocos2dx_extension_Manifest_getManifestFileUrl);
        tolua_function(tolua_S,"isVersionLoaded",lua_cocos2dx_extension_Manifest_isVersionLoaded);
        tolua_function(tolua_S,"isLoaded",lua_cocos2dx_extension_Manifest_isLoaded);
        tolua_function(tolua_S,"getPackageUrl",lua_cocos2dx_extension_Manifest_getPackageUrl);
        tolua_function(tolua_S,"getVersion",lua_cocos2dx_extension_Manifest_getVersion);
        tolua_function(tolua_S,"getVersionFileUrl",lua_cocos2dx_extension_Manifest_getVersionFileUrl);
        tolua_function(tolua_S,"getSearchPaths",lua_cocos2dx_extension_Manifest_getSearchPaths);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::Manifest).name();
    g_luaType[typeName] = "cc.Manifest";
    g_typeCast["Manifest"] = "cc.Manifest";
    return 1;
}

int lua_cocos2dx_extension_AssetsManagerEx_getState(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getState'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getState();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getState",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getState'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask'", nullptr);
            return 0;
        }
        const int ret = cobj->getMaxConcurrentTask();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getMaxConcurrentTask",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_checkUpdate(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_checkUpdate'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_checkUpdate'", nullptr);
            return 0;
        }
        cobj->checkUpdate();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:checkUpdate",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_checkUpdate'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<bool (const std::string&, cocos2d::extension::ManifestAsset)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback'", nullptr);
            return 0;
        }
        cobj->setVerifyCallback(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:setVerifyCallback",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getStoragePath(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getStoragePath'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getStoragePath'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getStoragePath();
        lua_pushlstring(tolua_S,ret.c_str(),ret.length());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getStoragePath",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getStoragePath'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_update(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_update'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_update'", nullptr);
            return 0;
        }
        cobj->update();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:update",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_update'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<int (const std::string&, const std::string&)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle'", nullptr);
            return 0;
        }
        cobj->setVersionCompareHandle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:setVersionCompareHandle",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "cc.AssetsManagerEx:setMaxConcurrentTask");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask'", nullptr);
            return 0;
        }
        cobj->setMaxConcurrentTask(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:setMaxConcurrentTask",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest'", nullptr);
            return 0;
        }
        const cocos2d::extension::Manifest* ret = cobj->getLocalManifest();
        object_to_luaval<cocos2d::extension::Manifest>(tolua_S, "cc.Manifest",(cocos2d::extension::Manifest*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getLocalManifest",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest'", nullptr);
            return 0;
        }
        const cocos2d::extension::Manifest* ret = cobj->getRemoteManifest();
        object_to_luaval<cocos2d::extension::Manifest>(tolua_S, "cc.Manifest",(cocos2d::extension::Manifest*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:getRemoteManifest",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::AssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets'", nullptr);
            return 0;
        }
        cobj->downloadFailedAssets();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:downloadFailedAssets",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"cc.AssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        std::string arg0;
        std::string arg1;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.AssetsManagerEx:create");
        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "cc.AssetsManagerEx:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_create'", nullptr);
            return 0;
        }
        cocos2d::extension::AssetsManagerEx* ret = cocos2d::extension::AssetsManagerEx::create(arg0, arg1);
        object_to_luaval<cocos2d::extension::AssetsManagerEx>(tolua_S, "cc.AssetsManagerEx",(cocos2d::extension::AssetsManagerEx*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "cc.AssetsManagerEx:create",argc, 2);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_extension_AssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::AssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "cc.AssetsManagerEx:AssetsManagerEx");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "cc.AssetsManagerEx:AssetsManagerEx");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_AssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::AssetsManagerEx(arg0, arg1);
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.AssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.AssetsManagerEx:AssetsManagerEx",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_AssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_AssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (AssetsManagerEx)");
    return 0;
}

int lua_register_cocos2dx_extension_AssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.AssetsManagerEx");
    tolua_cclass(tolua_S,"AssetsManagerEx","cc.AssetsManagerEx","cc.Ref",nullptr);

    tolua_beginmodule(tolua_S,"AssetsManagerEx");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_AssetsManagerEx_constructor);
        tolua_function(tolua_S,"getState",lua_cocos2dx_extension_AssetsManagerEx_getState);
        tolua_function(tolua_S,"getMaxConcurrentTask",lua_cocos2dx_extension_AssetsManagerEx_getMaxConcurrentTask);
        tolua_function(tolua_S,"checkUpdate",lua_cocos2dx_extension_AssetsManagerEx_checkUpdate);
        tolua_function(tolua_S,"setVerifyCallback",lua_cocos2dx_extension_AssetsManagerEx_setVerifyCallback);
        tolua_function(tolua_S,"getStoragePath",lua_cocos2dx_extension_AssetsManagerEx_getStoragePath);
        tolua_function(tolua_S,"update",lua_cocos2dx_extension_AssetsManagerEx_update);
        tolua_function(tolua_S,"setVersionCompareHandle",lua_cocos2dx_extension_AssetsManagerEx_setVersionCompareHandle);
        tolua_function(tolua_S,"setMaxConcurrentTask",lua_cocos2dx_extension_AssetsManagerEx_setMaxConcurrentTask);
        tolua_function(tolua_S,"getLocalManifest",lua_cocos2dx_extension_AssetsManagerEx_getLocalManifest);
        tolua_function(tolua_S,"getRemoteManifest",lua_cocos2dx_extension_AssetsManagerEx_getRemoteManifest);
        tolua_function(tolua_S,"downloadFailedAssets",lua_cocos2dx_extension_AssetsManagerEx_downloadFailedAssets);
        tolua_function(tolua_S,"create", lua_cocos2dx_extension_AssetsManagerEx_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::AssetsManagerEx).name();
    g_luaType[typeName] = "cc.AssetsManagerEx";
    g_typeCast["AssetsManagerEx"] = "cc.AssetsManagerEx";
    return 1;
}

int lua_cocos2dx_extension_EventListenerAssetsManagerEx_init(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventListenerAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"cc.EventListenerAssetsManagerEx",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::extension::EventListenerAssetsManagerEx*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_init'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const cocos2d::extension::AssetsManagerEx* arg0;
        std::function<void (cocos2d::extension::EventAssetsManagerEx *)> arg1;

        ok &= luaval_to_object<const cocos2d::extension::AssetsManagerEx>(tolua_S, 2, "cc.AssetsManagerEx",&arg0, "cc.EventListenerAssetsManagerEx:init");

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_init'", nullptr);
            return 0;
        }
        bool ret = cobj->init(arg0, arg1);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventListenerAssetsManagerEx:init",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_init'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::extension::EventListenerAssetsManagerEx* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::extension::EventListenerAssetsManagerEx();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"cc.EventListenerAssetsManagerEx");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "cc.EventListenerAssetsManagerEx:EventListenerAssetsManagerEx",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_extension_EventListenerAssetsManagerEx_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EventListenerAssetsManagerEx)");
    return 0;
}

int lua_register_cocos2dx_extension_EventListenerAssetsManagerEx(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"cc.EventListenerAssetsManagerEx");
    tolua_cclass(tolua_S,"EventListenerAssetsManagerEx","cc.EventListenerAssetsManagerEx","cc.EventListenerCustom",nullptr);

    tolua_beginmodule(tolua_S,"EventListenerAssetsManagerEx");
        tolua_function(tolua_S,"new",lua_cocos2dx_extension_EventListenerAssetsManagerEx_constructor);
        tolua_function(tolua_S,"init",lua_cocos2dx_extension_EventListenerAssetsManagerEx_init);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::extension::EventListenerAssetsManagerEx).name();
    g_luaType[typeName] = "cc.EventListenerAssetsManagerEx";
    g_typeCast["EventListenerAssetsManagerEx"] = "cc.EventListenerAssetsManagerEx";
    return 1;
}
TOLUA_API int register_all_cocos2dx_extension(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"cc",0);
	tolua_beginmodule(tolua_S,"cc");

	lua_register_cocos2dx_extension_AssetsManagerEx(tolua_S);
	lua_register_cocos2dx_extension_EventListenerAssetsManagerEx(tolua_S);
	lua_register_cocos2dx_extension_Manifest(tolua_S);
	lua_register_cocos2dx_extension_EventAssetsManagerEx(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

