#include "lua_cocos2dx_ui_auto.hpp"
#include "cocos_CocosGUI.h"
#include "lua_cocos2dx_tolua_fix.h"
#include "lua_cocos2dx_LuaBasicConversions.h"



int lua_cocos2dx_ui_LayoutParameter_clone(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LayoutParameter_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LayoutParameter_clone'", nullptr);
            return 0;
        }
        cocos2d::ui::LayoutParameter* ret = cobj->clone();
        object_to_luaval<cocos2d::ui::LayoutParameter>(tolua_S, "ccui.LayoutParameter",(cocos2d::ui::LayoutParameter*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LayoutParameter:clone",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LayoutParameter_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LayoutParameter_getLayoutType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LayoutParameter_getLayoutType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LayoutParameter_getLayoutType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getLayoutType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LayoutParameter:getLayoutType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LayoutParameter_getLayoutType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LayoutParameter_createCloneInstance(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LayoutParameter_createCloneInstance'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LayoutParameter_createCloneInstance'", nullptr);
            return 0;
        }
        cocos2d::ui::LayoutParameter* ret = cobj->createCloneInstance();
        object_to_luaval<cocos2d::ui::LayoutParameter>(tolua_S, "ccui.LayoutParameter",(cocos2d::ui::LayoutParameter*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LayoutParameter:createCloneInstance",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LayoutParameter_createCloneInstance'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LayoutParameter_copyProperties(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LayoutParameter_copyProperties'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::LayoutParameter* arg0;

        ok &= luaval_to_object<cocos2d::ui::LayoutParameter>(tolua_S, 2, "ccui.LayoutParameter",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LayoutParameter_copyProperties'", nullptr);
            return 0;
        }
        cobj->copyProperties(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LayoutParameter:copyProperties",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LayoutParameter_copyProperties'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LayoutParameter_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.LayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LayoutParameter_create'", nullptr);
            return 0;
        }
        cocos2d::ui::LayoutParameter* ret = cocos2d::ui::LayoutParameter::create();
        object_to_luaval<cocos2d::ui::LayoutParameter>(tolua_S, "ccui.LayoutParameter",(cocos2d::ui::LayoutParameter*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.LayoutParameter:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LayoutParameter_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_LayoutParameter_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LayoutParameter_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::LayoutParameter();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.LayoutParameter");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LayoutParameter:LayoutParameter",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LayoutParameter_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_LayoutParameter_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (LayoutParameter)");
    return 0;
}

int lua_register_cocos2dx_ui_LayoutParameter(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.LayoutParameter");
    tolua_cclass(tolua_S,"LayoutParameter","ccui.LayoutParameter","cc.Ref",nullptr);

    tolua_beginmodule(tolua_S,"LayoutParameter");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_LayoutParameter_constructor);
        tolua_function(tolua_S,"clone",lua_cocos2dx_ui_LayoutParameter_clone);
        tolua_function(tolua_S,"getLayoutType",lua_cocos2dx_ui_LayoutParameter_getLayoutType);
        tolua_function(tolua_S,"createCloneInstance",lua_cocos2dx_ui_LayoutParameter_createCloneInstance);
        tolua_function(tolua_S,"copyProperties",lua_cocos2dx_ui_LayoutParameter_copyProperties);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_LayoutParameter_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::LayoutParameter).name();
    g_luaType[typeName] = "ccui.LayoutParameter";
    g_typeCast["LayoutParameter"] = "ccui.LayoutParameter";
    return 1;
}

int lua_cocos2dx_ui_LinearLayoutParameter_setGravity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LinearLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LinearLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LinearLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LinearLayoutParameter_setGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::LinearLayoutParameter::LinearGravity arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.LinearLayoutParameter:setGravity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LinearLayoutParameter_setGravity'", nullptr);
            return 0;
        }
        cobj->setGravity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LinearLayoutParameter:setGravity",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LinearLayoutParameter_setGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LinearLayoutParameter_getGravity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LinearLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LinearLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LinearLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LinearLayoutParameter_getGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LinearLayoutParameter_getGravity'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getGravity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LinearLayoutParameter:getGravity",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LinearLayoutParameter_getGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LinearLayoutParameter_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.LinearLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LinearLayoutParameter_create'", nullptr);
            return 0;
        }
        cocos2d::ui::LinearLayoutParameter* ret = cocos2d::ui::LinearLayoutParameter::create();
        object_to_luaval<cocos2d::ui::LinearLayoutParameter>(tolua_S, "ccui.LinearLayoutParameter",(cocos2d::ui::LinearLayoutParameter*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.LinearLayoutParameter:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LinearLayoutParameter_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_LinearLayoutParameter_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LinearLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LinearLayoutParameter_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::LinearLayoutParameter();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.LinearLayoutParameter");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LinearLayoutParameter:LinearLayoutParameter",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LinearLayoutParameter_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_LinearLayoutParameter_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (LinearLayoutParameter)");
    return 0;
}

int lua_register_cocos2dx_ui_LinearLayoutParameter(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.LinearLayoutParameter");
    tolua_cclass(tolua_S,"LinearLayoutParameter","ccui.LinearLayoutParameter","ccui.LayoutParameter",nullptr);

    tolua_beginmodule(tolua_S,"LinearLayoutParameter");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_LinearLayoutParameter_constructor);
        tolua_function(tolua_S,"setGravity",lua_cocos2dx_ui_LinearLayoutParameter_setGravity);
        tolua_function(tolua_S,"getGravity",lua_cocos2dx_ui_LinearLayoutParameter_getGravity);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_LinearLayoutParameter_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::LinearLayoutParameter).name();
    g_luaType[typeName] = "ccui.LinearLayoutParameter";
    g_typeCast["LinearLayoutParameter"] = "ccui.LinearLayoutParameter";
    return 1;
}

int lua_cocos2dx_ui_RelativeLayoutParameter_setAlign(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RelativeLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RelativeLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::RelativeLayoutParameter::RelativeAlign arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.RelativeLayoutParameter:setAlign");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setAlign'", nullptr);
            return 0;
        }
        cobj->setAlign(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeLayoutParameter:setAlign",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeToWidgetName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RelativeLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RelativeLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeToWidgetName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.RelativeLayoutParameter:setRelativeToWidgetName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeToWidgetName'", nullptr);
            return 0;
        }
        cobj->setRelativeToWidgetName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeLayoutParameter:setRelativeToWidgetName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeToWidgetName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RelativeLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RelativeLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeName'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getRelativeName();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeLayoutParameter:getRelativeName",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeToWidgetName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RelativeLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RelativeLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeToWidgetName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeToWidgetName'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getRelativeToWidgetName();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeLayoutParameter:getRelativeToWidgetName",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeToWidgetName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RelativeLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RelativeLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.RelativeLayoutParameter:setRelativeName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeName'", nullptr);
            return 0;
        }
        cobj->setRelativeName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeLayoutParameter:setRelativeName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RelativeLayoutParameter_getAlign(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RelativeLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RelativeLayoutParameter*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getAlign'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getAlign'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getAlign();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeLayoutParameter:getAlign",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_getAlign'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RelativeLayoutParameter_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.RelativeLayoutParameter",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_create'", nullptr);
            return 0;
        }
        cocos2d::ui::RelativeLayoutParameter* ret = cocos2d::ui::RelativeLayoutParameter::create();
        object_to_luaval<cocos2d::ui::RelativeLayoutParameter>(tolua_S, "ccui.RelativeLayoutParameter",(cocos2d::ui::RelativeLayoutParameter*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.RelativeLayoutParameter:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_RelativeLayoutParameter_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeLayoutParameter* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeLayoutParameter_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::RelativeLayoutParameter();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.RelativeLayoutParameter");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeLayoutParameter:RelativeLayoutParameter",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeLayoutParameter_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_RelativeLayoutParameter_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (RelativeLayoutParameter)");
    return 0;
}

int lua_register_cocos2dx_ui_RelativeLayoutParameter(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RelativeLayoutParameter");
    tolua_cclass(tolua_S,"RelativeLayoutParameter","ccui.RelativeLayoutParameter","ccui.LayoutParameter",nullptr);

    tolua_beginmodule(tolua_S,"RelativeLayoutParameter");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_RelativeLayoutParameter_constructor);
        tolua_function(tolua_S,"setAlign",lua_cocos2dx_ui_RelativeLayoutParameter_setAlign);
        tolua_function(tolua_S,"setRelativeToWidgetName",lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeToWidgetName);
        tolua_function(tolua_S,"getRelativeName",lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeName);
        tolua_function(tolua_S,"getRelativeToWidgetName",lua_cocos2dx_ui_RelativeLayoutParameter_getRelativeToWidgetName);
        tolua_function(tolua_S,"setRelativeName",lua_cocos2dx_ui_RelativeLayoutParameter_setRelativeName);
        tolua_function(tolua_S,"getAlign",lua_cocos2dx_ui_RelativeLayoutParameter_getAlign);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_RelativeLayoutParameter_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RelativeLayoutParameter).name();
    g_luaType[typeName] = "ccui.RelativeLayoutParameter";
    g_typeCast["RelativeLayoutParameter"] = "ccui.RelativeLayoutParameter";
    return 1;
}

int lua_cocos2dx_ui_Widget_setSizePercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setSizePercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.Widget:setSizePercent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setSizePercent'", nullptr);
            return 0;
        }
        cobj->setSizePercent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setSizePercent",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setSizePercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getCustomSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getCustomSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getCustomSize'", nullptr);
            return 0;
        }
        const cocos2d::Size& ret = cobj->getCustomSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getCustomSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getCustomSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getLeftBoundary(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getLeftBoundary'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getLeftBoundary'", nullptr);
            return 0;
        }
        double ret = cobj->getLeftBoundary();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getLeftBoundary",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getLeftBoundary'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setFlippedX(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setFlippedX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setFlippedX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setFlippedX'", nullptr);
            return 0;
        }
        cobj->setFlippedX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setFlippedX",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setFlippedX'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setCallbackName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setCallbackName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Widget:setCallbackName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setCallbackName'", nullptr);
            return 0;
        }
        cobj->setCallbackName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setCallbackName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setCallbackName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getVirtualRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getVirtualRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getVirtualRenderer'", nullptr);
            return 0;
        }
        cocos2d::Node* ret = cobj->getVirtualRenderer();
        object_to_luaval<cocos2d::Node>(tolua_S, "cc.Node",(cocos2d::Node*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getVirtualRenderer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getVirtualRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setPropagateTouchEvents(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setPropagateTouchEvents'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setPropagateTouchEvents");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setPropagateTouchEvents'", nullptr);
            return 0;
        }
        cobj->setPropagateTouchEvents(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setPropagateTouchEvents",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setPropagateTouchEvents'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isUnifySizeEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isUnifySizeEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isUnifySizeEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isUnifySizeEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isUnifySizeEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isUnifySizeEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getSizePercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getSizePercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getSizePercent'", nullptr);
            return 0;
        }
        const cocos2d::Vec2& ret = cobj->getSizePercent();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getSizePercent",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getSizePercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setPositionPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setPositionPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.Widget:setPositionPercent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setPositionPercent'", nullptr);
            return 0;
        }
        cobj->setPositionPercent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setPositionPercent",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setPositionPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setSwallowTouches(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setSwallowTouches'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setSwallowTouches");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setSwallowTouches'", nullptr);
            return 0;
        }
        cobj->setSwallowTouches(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setSwallowTouches",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setSwallowTouches'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getLayoutSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getLayoutSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getLayoutSize'", nullptr);
            return 0;
        }
        const cocos2d::Size& ret = cobj->getLayoutSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getLayoutSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getLayoutSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setHighlighted(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setHighlighted'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setHighlighted");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setHighlighted'", nullptr);
            return 0;
        }
        cobj->setHighlighted(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setHighlighted",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setHighlighted'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setPositionType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setPositionType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Widget::PositionType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Widget:setPositionType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setPositionType'", nullptr);
            return 0;
        }
        cobj->setPositionType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setPositionType",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setPositionType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isIgnoreContentAdaptWithSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isIgnoreContentAdaptWithSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isIgnoreContentAdaptWithSize'", nullptr);
            return 0;
        }
        bool ret = cobj->isIgnoreContentAdaptWithSize();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isIgnoreContentAdaptWithSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isIgnoreContentAdaptWithSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getVirtualRendererSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getVirtualRendererSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getVirtualRendererSize'", nullptr);
            return 0;
        }
        cocos2d::Size ret = cobj->getVirtualRendererSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getVirtualRendererSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getVirtualRendererSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isHighlighted(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isHighlighted'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isHighlighted'", nullptr);
            return 0;
        }
        bool ret = cobj->isHighlighted();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isHighlighted",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isHighlighted'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getLayoutParameter(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getLayoutParameter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getLayoutParameter'", nullptr);
            return 0;
        }
        cocos2d::ui::LayoutParameter* ret = cobj->getLayoutParameter();
        object_to_luaval<cocos2d::ui::LayoutParameter>(tolua_S, "ccui.LayoutParameter",(cocos2d::ui::LayoutParameter*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getLayoutParameter",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getLayoutParameter'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_addCCSEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_addCCSEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, int)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_addCCSEventListener'", nullptr);
            return 0;
        }
        cobj->addCCSEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:addCCSEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_addCCSEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getPositionType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getPositionType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getPositionType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getPositionType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getPositionType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getPositionType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getTopBoundary(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getTopBoundary'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getTopBoundary'", nullptr);
            return 0;
        }
        double ret = cobj->getTopBoundary();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getTopBoundary",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getTopBoundary'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_ignoreContentAdaptWithSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_ignoreContentAdaptWithSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:ignoreContentAdaptWithSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_ignoreContentAdaptWithSize'", nullptr);
            return 0;
        }
        cobj->ignoreContentAdaptWithSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:ignoreContentAdaptWithSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_ignoreContentAdaptWithSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_findNextFocusedWidget(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_findNextFocusedWidget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        cocos2d::ui::Widget::FocusDirection arg0;
        cocos2d::ui::Widget* arg1;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Widget:findNextFocusedWidget");

        ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 3, "ccui.Widget",&arg1);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_findNextFocusedWidget'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cobj->findNextFocusedWidget(arg0, arg1);
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:findNextFocusedWidget",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_findNextFocusedWidget'.",&tolua_err);
#endif

    return 0;
}

int lua_cocos2dx_ui_Widget_isEnabled(lua_State* tolua_S)
{
    cocos2d::ui::Widget *cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S, 1, 0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isEnabled'", nullptr);
        return 0;
    }
#endif

    int checkParent = lua_toboolean(tolua_S, 2);
    bool ret = cobj->isEnabled(checkParent);
    tolua_pushboolean(tolua_S, (bool)ret);
    return 1;
}

int lua_cocos2dx_ui_Widget_isFocused(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isFocused'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isFocused'", nullptr);
            return 0;
        }
        bool ret = cobj->isFocused();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isFocused",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isFocused'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getTouchBeganPosition(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getTouchBeganPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getTouchBeganPosition'", nullptr);
            return 0;
        }
        const cocos2d::Vec2& ret = cobj->getTouchBeganPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getTouchBeganPosition",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getTouchBeganPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isTouchEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isTouchEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isTouchEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isTouchEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isTouchEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isTouchEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getCallbackName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getCallbackName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getCallbackName'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getCallbackName();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getCallbackName",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getCallbackName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getActionTag(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getActionTag'", nullptr);
            return 0;
        }
        int ret = cobj->getActionTag();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getActionTag",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getWorldPosition(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getWorldPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getWorldPosition'", nullptr);
            return 0;
        }
        cocos2d::Vec2 ret = cobj->getWorldPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getWorldPosition",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getWorldPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isFocusEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isFocusEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isFocusEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isFocusEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isFocusEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isFocusEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setFocused(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setFocused'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setFocused");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setFocused'", nullptr);
            return 0;
        }
        cobj->setFocused(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setFocused",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setFocused'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setActionTag(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setActionTag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Widget:setActionTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setActionTag'", nullptr);
            return 0;
        }
        cobj->setActionTag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setActionTag",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setActionTag'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setTouchEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setTouchEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setTouchEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setTouchEnabled'", nullptr);
            return 0;
        }
        cobj->setTouchEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setTouchEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setTouchEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setFlippedY(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setFlippedY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setFlippedY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setFlippedY'", nullptr);
            return 0;
        }
        cobj->setFlippedY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setFlippedY",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setFlippedY'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setEnabled'", nullptr);
            return 0;
        }
        cobj->setEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getRightBoundary(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getRightBoundary'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getRightBoundary'", nullptr);
            return 0;
        }
        double ret = cobj->getRightBoundary();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getRightBoundary",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getRightBoundary'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setBrightStyle(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setBrightStyle'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Widget::BrightStyle arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Widget:setBrightStyle");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setBrightStyle'", nullptr);
            return 0;
        }
        cobj->setBrightStyle(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setBrightStyle",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setBrightStyle'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setLayoutParameter(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setLayoutParameter'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::LayoutParameter* arg0;

        ok &= luaval_to_object<cocos2d::ui::LayoutParameter>(tolua_S, 2, "ccui.LayoutParameter",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setLayoutParameter'", nullptr);
            return 0;
        }
        cobj->setLayoutParameter(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setLayoutParameter",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setLayoutParameter'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_clone(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_clone'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_clone'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cobj->clone();
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:clone",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_clone'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setFocusEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setFocusEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setFocusEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setFocusEnabled'", nullptr);
            return 0;
        }
        cobj->setFocusEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setFocusEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setFocusEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getBottomBoundary(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getBottomBoundary'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getBottomBoundary'", nullptr);
            return 0;
        }
        double ret = cobj->getBottomBoundary();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getBottomBoundary",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getBottomBoundary'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isBright(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isBright'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isBright'", nullptr);
            return 0;
        }
        bool ret = cobj->isBright();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isBright",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isBright'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setUnifySizeEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setUnifySizeEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setUnifySizeEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setUnifySizeEnabled'", nullptr);
            return 0;
        }
        cobj->setUnifySizeEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setUnifySizeEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setUnifySizeEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isPropagateTouchEvents(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isPropagateTouchEvents'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isPropagateTouchEvents'", nullptr);
            return 0;
        }
        bool ret = cobj->isPropagateTouchEvents();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isPropagateTouchEvents",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isPropagateTouchEvents'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getCurrentFocusedWidget(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getCurrentFocusedWidget'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getCurrentFocusedWidget'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cobj->getCurrentFocusedWidget();
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getCurrentFocusedWidget",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getCurrentFocusedWidget'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_hitTest(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_hitTest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.Widget:hitTest");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_hitTest'", nullptr);
            return 0;
        }
        bool ret = cobj->hitTest(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:hitTest",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_hitTest'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_requestFocus(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_requestFocus'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_requestFocus'", nullptr);
            return 0;
        }
        cobj->requestFocus();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:requestFocus",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_requestFocus'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_updateSizeAndPosition(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_updateSizeAndPosition'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            cocos2d::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.Widget:updateSizeAndPosition");

            if (!ok) { break; }
            cobj->updateSizeAndPosition(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            cobj->updateSizeAndPosition();
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.Widget:updateSizeAndPosition",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_updateSizeAndPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getTouchMovePosition(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getTouchMovePosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getTouchMovePosition'", nullptr);
            return 0;
        }
        const cocos2d::Vec2& ret = cobj->getTouchMovePosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getTouchMovePosition",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getTouchMovePosition'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getSizeType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getSizeType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getSizeType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getSizeType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getSizeType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getSizeType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getCallbackType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getCallbackType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getCallbackType'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getCallbackType();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getCallbackType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getCallbackType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_addTouchEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_addTouchEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, cocos2d::ui::Widget::TouchEventType)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_addTouchEventListener'", nullptr);
            return 0;
        }
        cobj->addTouchEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:addTouchEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_addTouchEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getTouchEndPosition(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getTouchEndPosition'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getTouchEndPosition'", nullptr);
            return 0;
        }
        const cocos2d::Vec2& ret = cobj->getTouchEndPosition();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getTouchEndPosition",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getTouchEndPosition'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_getPositionPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_getPositionPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_getPositionPercent'", nullptr);
            return 0;
        }
        const cocos2d::Vec2& ret = cobj->getPositionPercent();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:getPositionPercent",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_getPositionPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_addClickEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_addClickEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_addClickEventListener'", nullptr);
            return 0;
        }
        cobj->addClickEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:addClickEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_addClickEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isFlippedX(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isFlippedX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isFlippedX'", nullptr);
            return 0;
        }
        bool ret = cobj->isFlippedX();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isFlippedX",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isFlippedX'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isFlippedY(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isFlippedY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isFlippedY'", nullptr);
            return 0;
        }
        bool ret = cobj->isFlippedY();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isFlippedY",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isFlippedY'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isClippingParentContainsPoint(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isClippingParentContainsPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.Widget:isClippingParentContainsPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isClippingParentContainsPoint'", nullptr);
            return 0;
        }
        bool ret = cobj->isClippingParentContainsPoint(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isClippingParentContainsPoint",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isClippingParentContainsPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setSizeType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setSizeType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Widget::SizeType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Widget:setSizeType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setSizeType'", nullptr);
            return 0;
        }
        cobj->setSizeType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setSizeType",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setSizeType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setBright(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setBright'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:setBright");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setBright'", nullptr);
            return 0;
        }
        cobj->setBright(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setBright",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setBright'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_setCallbackType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_setCallbackType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Widget:setCallbackType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_setCallbackType'", nullptr);
            return 0;
        }
        cobj->setCallbackType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:setCallbackType",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_setCallbackType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_isSwallowTouches(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Widget*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Widget_isSwallowTouches'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_isSwallowTouches'", nullptr);
            return 0;
        }
        bool ret = cobj->isSwallowTouches();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:isSwallowTouches",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_isSwallowTouches'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Widget_enableDpadNavigation(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        bool arg0;
        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Widget:enableDpadNavigation");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_enableDpadNavigation'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget::enableDpadNavigation(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Widget:enableDpadNavigation",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_enableDpadNavigation'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Widget_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Widget",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_create'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cocos2d::ui::Widget::create();
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Widget:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Widget_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Widget* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Widget_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::Widget();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.Widget");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Widget:Widget",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Widget_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_Widget_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Widget)");
    return 0;
}

int lua_register_cocos2dx_ui_Widget(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.Widget");
    tolua_cclass(tolua_S,"Widget","ccui.Widget","cc.ProtectedNode",nullptr);

    tolua_beginmodule(tolua_S,"Widget");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_Widget_constructor);
        tolua_function(tolua_S,"setSizePercent",lua_cocos2dx_ui_Widget_setSizePercent);
        tolua_function(tolua_S,"getCustomSize",lua_cocos2dx_ui_Widget_getCustomSize);
        tolua_function(tolua_S,"getLeftBoundary",lua_cocos2dx_ui_Widget_getLeftBoundary);
        tolua_function(tolua_S,"setFlippedX",lua_cocos2dx_ui_Widget_setFlippedX);
        tolua_function(tolua_S,"setCallbackName",lua_cocos2dx_ui_Widget_setCallbackName);
        tolua_function(tolua_S,"getVirtualRenderer",lua_cocos2dx_ui_Widget_getVirtualRenderer);
        tolua_function(tolua_S,"setPropagateTouchEvents",lua_cocos2dx_ui_Widget_setPropagateTouchEvents);
        tolua_function(tolua_S,"isUnifySizeEnabled",lua_cocos2dx_ui_Widget_isUnifySizeEnabled);
        tolua_function(tolua_S,"getSizePercent",lua_cocos2dx_ui_Widget_getSizePercent);
        tolua_function(tolua_S,"setPositionPercent",lua_cocos2dx_ui_Widget_setPositionPercent);
        tolua_function(tolua_S,"setSwallowTouches",lua_cocos2dx_ui_Widget_setSwallowTouches);
        tolua_function(tolua_S,"getLayoutSize",lua_cocos2dx_ui_Widget_getLayoutSize);
        tolua_function(tolua_S,"setHighlighted",lua_cocos2dx_ui_Widget_setHighlighted);
        tolua_function(tolua_S,"setPositionType",lua_cocos2dx_ui_Widget_setPositionType);
        tolua_function(tolua_S,"isIgnoreContentAdaptWithSize",lua_cocos2dx_ui_Widget_isIgnoreContentAdaptWithSize);
        tolua_function(tolua_S,"getVirtualRendererSize",lua_cocos2dx_ui_Widget_getVirtualRendererSize);
        tolua_function(tolua_S,"isHighlighted",lua_cocos2dx_ui_Widget_isHighlighted);
        tolua_function(tolua_S,"getLayoutParameter",lua_cocos2dx_ui_Widget_getLayoutParameter);
        tolua_function(tolua_S,"addCCSEventListener",lua_cocos2dx_ui_Widget_addCCSEventListener);
        tolua_function(tolua_S,"getPositionType",lua_cocos2dx_ui_Widget_getPositionType);
        tolua_function(tolua_S,"getTopBoundary",lua_cocos2dx_ui_Widget_getTopBoundary);
        tolua_function(tolua_S,"ignoreContentAdaptWithSize",lua_cocos2dx_ui_Widget_ignoreContentAdaptWithSize);
        tolua_function(tolua_S,"findNextFocusedWidget",lua_cocos2dx_ui_Widget_findNextFocusedWidget);
        tolua_function(tolua_S,"isEnabled",lua_cocos2dx_ui_Widget_isEnabled);
        tolua_function(tolua_S,"isFocused",lua_cocos2dx_ui_Widget_isFocused);
        tolua_function(tolua_S,"getTouchBeganPosition",lua_cocos2dx_ui_Widget_getTouchBeganPosition);
        tolua_function(tolua_S,"isTouchEnabled",lua_cocos2dx_ui_Widget_isTouchEnabled);
        tolua_function(tolua_S,"getCallbackName",lua_cocos2dx_ui_Widget_getCallbackName);
        tolua_function(tolua_S,"getActionTag",lua_cocos2dx_ui_Widget_getActionTag);
        tolua_function(tolua_S,"getWorldPosition",lua_cocos2dx_ui_Widget_getWorldPosition);
        tolua_function(tolua_S,"isFocusEnabled",lua_cocos2dx_ui_Widget_isFocusEnabled);
        tolua_function(tolua_S,"setFocused",lua_cocos2dx_ui_Widget_setFocused);
        tolua_function(tolua_S,"setActionTag",lua_cocos2dx_ui_Widget_setActionTag);
        tolua_function(tolua_S,"setTouchEnabled",lua_cocos2dx_ui_Widget_setTouchEnabled);
        tolua_function(tolua_S,"setFlippedY",lua_cocos2dx_ui_Widget_setFlippedY);
        tolua_function(tolua_S,"setEnabled",lua_cocos2dx_ui_Widget_setEnabled);
        tolua_function(tolua_S,"getRightBoundary",lua_cocos2dx_ui_Widget_getRightBoundary);
        tolua_function(tolua_S,"setBrightStyle",lua_cocos2dx_ui_Widget_setBrightStyle);
        tolua_function(tolua_S,"setLayoutParameter",lua_cocos2dx_ui_Widget_setLayoutParameter);
        tolua_function(tolua_S,"clone",lua_cocos2dx_ui_Widget_clone);
        tolua_function(tolua_S,"setFocusEnabled",lua_cocos2dx_ui_Widget_setFocusEnabled);
        tolua_function(tolua_S,"getBottomBoundary",lua_cocos2dx_ui_Widget_getBottomBoundary);
        tolua_function(tolua_S,"isBright",lua_cocos2dx_ui_Widget_isBright);
        tolua_function(tolua_S,"setUnifySizeEnabled",lua_cocos2dx_ui_Widget_setUnifySizeEnabled);
        tolua_function(tolua_S,"isPropagateTouchEvents",lua_cocos2dx_ui_Widget_isPropagateTouchEvents);
        tolua_function(tolua_S,"getCurrentFocusedWidget",lua_cocos2dx_ui_Widget_getCurrentFocusedWidget);
        tolua_function(tolua_S,"hitTest",lua_cocos2dx_ui_Widget_hitTest);
        tolua_function(tolua_S,"requestFocus",lua_cocos2dx_ui_Widget_requestFocus);
        tolua_function(tolua_S,"updateSizeAndPosition",lua_cocos2dx_ui_Widget_updateSizeAndPosition);
        tolua_function(tolua_S,"getTouchMovePosition",lua_cocos2dx_ui_Widget_getTouchMovePosition);
        tolua_function(tolua_S,"getSizeType",lua_cocos2dx_ui_Widget_getSizeType);
        tolua_function(tolua_S,"getCallbackType",lua_cocos2dx_ui_Widget_getCallbackType);
        tolua_function(tolua_S,"addTouchEventListener",lua_cocos2dx_ui_Widget_addTouchEventListener);
        tolua_function(tolua_S,"getTouchEndPosition",lua_cocos2dx_ui_Widget_getTouchEndPosition);
        tolua_function(tolua_S,"getPositionPercent",lua_cocos2dx_ui_Widget_getPositionPercent);
        tolua_function(tolua_S,"addClickEventListener",lua_cocos2dx_ui_Widget_addClickEventListener);
        tolua_function(tolua_S,"isFlippedX",lua_cocos2dx_ui_Widget_isFlippedX);
        tolua_function(tolua_S,"isFlippedY",lua_cocos2dx_ui_Widget_isFlippedY);
        tolua_function(tolua_S,"isClippingParentContainsPoint",lua_cocos2dx_ui_Widget_isClippingParentContainsPoint);
        tolua_function(tolua_S,"setSizeType",lua_cocos2dx_ui_Widget_setSizeType);
        tolua_function(tolua_S,"setBright",lua_cocos2dx_ui_Widget_setBright);
        tolua_function(tolua_S,"setCallbackType",lua_cocos2dx_ui_Widget_setCallbackType);
        tolua_function(tolua_S,"isSwallowTouches",lua_cocos2dx_ui_Widget_isSwallowTouches);
        tolua_function(tolua_S,"enableDpadNavigation", lua_cocos2dx_ui_Widget_enableDpadNavigation);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_Widget_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::Widget).name();
    g_luaType[typeName] = "ccui.Widget";
    g_typeCast["Widget"] = "ccui.Widget";
    return 1;
}

int lua_cocos2dx_ui_Layout_setBackGroundColorVector(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundColorVector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.Layout:setBackGroundColorVector");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundColorVector'", nullptr);
            return 0;
        }
        cobj->setBackGroundColorVector(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundColorVector",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundColorVector'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setClippingType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setClippingType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Layout::ClippingType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Layout:setClippingType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setClippingType'", nullptr);
            return 0;
        }
        cobj->setClippingType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setClippingType",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setClippingType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundColorType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundColorType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Layout::BackGroundColorType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Layout:setBackGroundColorType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundColorType'", nullptr);
            return 0;
        }
        cobj->setBackGroundColorType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundColorType",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundColorType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setLoopFocus(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setLoopFocus'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Layout:setLoopFocus");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setLoopFocus'", nullptr);
            return 0;
        }
        cobj->setLoopFocus(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setLoopFocus",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setLoopFocus'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundImageColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundImageColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "ccui.Layout:setBackGroundImageColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundImageColor'", nullptr);
            return 0;
        }
        cobj->setBackGroundImageColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundImageColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundImageColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundColorVector(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundColorVector'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundColorVector'", nullptr);
            return 0;
        }
        const cocos2d::Vec2& ret = cobj->getBackGroundColorVector();
        vec2_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundColorVector",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundColorVector'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getClippingType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getClippingType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getClippingType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getClippingType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getClippingType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getClippingType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_isLoopFocus(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_isLoopFocus'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_isLoopFocus'", nullptr);
            return 0;
        }
        bool ret = cobj->isLoopFocus();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:isLoopFocus",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_isLoopFocus'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_removeBackGroundImage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_removeBackGroundImage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_removeBackGroundImage'", nullptr);
            return 0;
        }
        cobj->removeBackGroundImage();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:removeBackGroundImage",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_removeBackGroundImage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundColorOpacity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundColorOpacity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundColorOpacity'", nullptr);
            return 0;
        }
        uint16_t ret = cobj->getBackGroundColorOpacity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundColorOpacity",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundColorOpacity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_isClippingEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_isClippingEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_isClippingEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isClippingEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:isClippingEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_isClippingEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundImageOpacity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundImageOpacity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        uint16_t arg0;

        ok &= luaval_to_uint16(tolua_S, 2,&arg0, "ccui.Layout:setBackGroundImageOpacity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundImageOpacity'", nullptr);
            return 0;
        }
        cobj->setBackGroundImageOpacity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundImageOpacity",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundImageOpacity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundImage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundImage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Layout:setBackGroundImage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundImage'", nullptr);
            return 0;
        }
        cobj->setBackGroundImage(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Layout:setBackGroundImage");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Layout:setBackGroundImage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundImage'", nullptr);
            return 0;
        }
        cobj->setBackGroundImage(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundImage",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundImage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundColor'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            cocos2d::Color3B arg0;
            ok &= luaval_to_color3b(tolua_S, 2, &arg0, "ccui.Layout:setBackGroundColor");

            if (!ok) { break; }
            cocos2d::Color3B arg1;
            ok &= luaval_to_color3b(tolua_S, 3, &arg1, "ccui.Layout:setBackGroundColor");

            if (!ok) { break; }
            cobj->setBackGroundColor(arg0, arg1);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            cocos2d::Color3B arg0;
            ok &= luaval_to_color3b(tolua_S, 2, &arg0, "ccui.Layout:setBackGroundColor");

            if (!ok) { break; }
            cobj->setBackGroundColor(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.Layout:setBackGroundColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_requestDoLayout(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_requestDoLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_requestDoLayout'", nullptr);
            return 0;
        }
        cobj->requestDoLayout();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:requestDoLayout",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_requestDoLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundImageCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundImageCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundImageCapInsets'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getBackGroundImageCapInsets();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundImageCapInsets",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundImageCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundColor'", nullptr);
            return 0;
        }
        const cocos2d::Color3B& ret = cobj->getBackGroundColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setClippingEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setClippingEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Layout:setClippingEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setClippingEnabled'", nullptr);
            return 0;
        }
        cobj->setClippingEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setClippingEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setClippingEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundImageColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundImageColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundImageColor'", nullptr);
            return 0;
        }
        const cocos2d::Color3B& ret = cobj->getBackGroundImageColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundImageColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundImageColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_isBackGroundImageScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_isBackGroundImageScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_isBackGroundImageScale9Enabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isBackGroundImageScale9Enabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:isBackGroundImageScale9Enabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_isBackGroundImageScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundColorType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundColorType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundColorType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getBackGroundColorType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundColorType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundColorType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundEndColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundEndColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundEndColor'", nullptr);
            return 0;
        }
        const cocos2d::Color3B& ret = cobj->getBackGroundEndColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundEndColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundEndColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundColorOpacity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundColorOpacity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        uint16_t arg0;

        ok &= luaval_to_uint16(tolua_S, 2,&arg0, "ccui.Layout:setBackGroundColorOpacity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundColorOpacity'", nullptr);
            return 0;
        }
        cobj->setBackGroundColorOpacity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundColorOpacity",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundColorOpacity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundImageOpacity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundImageOpacity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundImageOpacity'", nullptr);
            return 0;
        }
        uint16_t ret = cobj->getBackGroundImageOpacity();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundImageOpacity",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundImageOpacity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_isPassFocusToChild(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_isPassFocusToChild'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_isPassFocusToChild'", nullptr);
            return 0;
        }
        bool ret = cobj->isPassFocusToChild();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:isPassFocusToChild",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_isPassFocusToChild'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundImageCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundImageCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Layout:setBackGroundImageCapInsets");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundImageCapInsets'", nullptr);
            return 0;
        }
        cobj->setBackGroundImageCapInsets(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundImageCapInsets",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundImageCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundImageTextureSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundImageTextureSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundImageTextureSize'", nullptr);
            return 0;
        }
        const cocos2d::Size& ret = cobj->getBackGroundImageTextureSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundImageTextureSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundImageTextureSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_forceDoLayout(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_forceDoLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_forceDoLayout'", nullptr);
            return 0;
        }
        cobj->forceDoLayout();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:forceDoLayout",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_forceDoLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getLayoutType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getLayoutType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getLayoutType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getLayoutType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getLayoutType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getLayoutType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setPassFocusToChild(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setPassFocusToChild'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Layout:setPassFocusToChild");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setPassFocusToChild'", nullptr);
            return 0;
        }
        cobj->setPassFocusToChild(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setPassFocusToChild",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setPassFocusToChild'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_getBackGroundStartColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_getBackGroundStartColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_getBackGroundStartColor'", nullptr);
            return 0;
        }
        const cocos2d::Color3B& ret = cobj->getBackGroundStartColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:getBackGroundStartColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_getBackGroundStartColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setBackGroundImageScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setBackGroundImageScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Layout:setBackGroundImageScale9Enabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setBackGroundImageScale9Enabled'", nullptr);
            return 0;
        }
        cobj->setBackGroundImageScale9Enabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setBackGroundImageScale9Enabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setBackGroundImageScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_setLayoutType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Layout*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Layout_setLayoutType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Layout::Type arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Layout:setLayoutType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_setLayoutType'", nullptr);
            return 0;
        }
        cobj->setLayoutType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:setLayoutType",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_setLayoutType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Layout_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_create'", nullptr);
            return 0;
        }
        cocos2d::ui::Layout* ret = cocos2d::ui::Layout::create();
        object_to_luaval<cocos2d::ui::Layout>(tolua_S, "ccui.Layout",(cocos2d::ui::Layout*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Layout:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Layout_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Layout",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::Layout::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Layout:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Layout_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Layout* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Layout_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::Layout();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.Layout");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Layout:Layout",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Layout_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_Layout_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Layout)");
    return 0;
}

int lua_register_cocos2dx_ui_Layout(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.Layout");
    tolua_cclass(tolua_S,"Layout","ccui.Layout","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"Layout");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_Layout_constructor);
        tolua_function(tolua_S,"setBackGroundColorVector",lua_cocos2dx_ui_Layout_setBackGroundColorVector);
        tolua_function(tolua_S,"setClippingType",lua_cocos2dx_ui_Layout_setClippingType);
        tolua_function(tolua_S,"setBackGroundColorType",lua_cocos2dx_ui_Layout_setBackGroundColorType);
        tolua_function(tolua_S,"setLoopFocus",lua_cocos2dx_ui_Layout_setLoopFocus);
        tolua_function(tolua_S,"setBackGroundImageColor",lua_cocos2dx_ui_Layout_setBackGroundImageColor);
        tolua_function(tolua_S,"getBackGroundColorVector",lua_cocos2dx_ui_Layout_getBackGroundColorVector);
        tolua_function(tolua_S,"getClippingType",lua_cocos2dx_ui_Layout_getClippingType);
        tolua_function(tolua_S,"isLoopFocus",lua_cocos2dx_ui_Layout_isLoopFocus);
        tolua_function(tolua_S,"removeBackGroundImage",lua_cocos2dx_ui_Layout_removeBackGroundImage);
        tolua_function(tolua_S,"getBackGroundColorOpacity",lua_cocos2dx_ui_Layout_getBackGroundColorOpacity);
        tolua_function(tolua_S,"isClippingEnabled",lua_cocos2dx_ui_Layout_isClippingEnabled);
        tolua_function(tolua_S,"setBackGroundImageOpacity",lua_cocos2dx_ui_Layout_setBackGroundImageOpacity);
        tolua_function(tolua_S,"setBackGroundImage",lua_cocos2dx_ui_Layout_setBackGroundImage);
        tolua_function(tolua_S,"setBackGroundColor",lua_cocos2dx_ui_Layout_setBackGroundColor);
        tolua_function(tolua_S,"requestDoLayout",lua_cocos2dx_ui_Layout_requestDoLayout);
        tolua_function(tolua_S,"getBackGroundImageCapInsets",lua_cocos2dx_ui_Layout_getBackGroundImageCapInsets);
        tolua_function(tolua_S,"getBackGroundColor",lua_cocos2dx_ui_Layout_getBackGroundColor);
        tolua_function(tolua_S,"setClippingEnabled",lua_cocos2dx_ui_Layout_setClippingEnabled);
        tolua_function(tolua_S,"getBackGroundImageColor",lua_cocos2dx_ui_Layout_getBackGroundImageColor);
        tolua_function(tolua_S,"isBackGroundImageScale9Enabled",lua_cocos2dx_ui_Layout_isBackGroundImageScale9Enabled);
        tolua_function(tolua_S,"getBackGroundColorType",lua_cocos2dx_ui_Layout_getBackGroundColorType);
        tolua_function(tolua_S,"getBackGroundEndColor",lua_cocos2dx_ui_Layout_getBackGroundEndColor);
        tolua_function(tolua_S,"setBackGroundColorOpacity",lua_cocos2dx_ui_Layout_setBackGroundColorOpacity);
        tolua_function(tolua_S,"getBackGroundImageOpacity",lua_cocos2dx_ui_Layout_getBackGroundImageOpacity);
        tolua_function(tolua_S,"isPassFocusToChild",lua_cocos2dx_ui_Layout_isPassFocusToChild);
        tolua_function(tolua_S,"setBackGroundImageCapInsets",lua_cocos2dx_ui_Layout_setBackGroundImageCapInsets);
        tolua_function(tolua_S,"getBackGroundImageTextureSize",lua_cocos2dx_ui_Layout_getBackGroundImageTextureSize);
        tolua_function(tolua_S,"forceDoLayout",lua_cocos2dx_ui_Layout_forceDoLayout);
        tolua_function(tolua_S,"getLayoutType",lua_cocos2dx_ui_Layout_getLayoutType);
        tolua_function(tolua_S,"setPassFocusToChild",lua_cocos2dx_ui_Layout_setPassFocusToChild);
        tolua_function(tolua_S,"getBackGroundStartColor",lua_cocos2dx_ui_Layout_getBackGroundStartColor);
        tolua_function(tolua_S,"setBackGroundImageScale9Enabled",lua_cocos2dx_ui_Layout_setBackGroundImageScale9Enabled);
        tolua_function(tolua_S,"setLayoutType",lua_cocos2dx_ui_Layout_setLayoutType);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_Layout_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_Layout_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::Layout).name();
    g_luaType[typeName] = "ccui.Layout";
    g_typeCast["Layout"] = "ccui.Layout";
    return 1;
}

int lua_cocos2dx_ui_Button_getTitleText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getTitleText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getTitleText'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getTitleText();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getTitleText",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getTitleText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setTitleFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setTitleFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.Button:setTitleFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setTitleFontSize'", nullptr);
            return 0;
        }
        cobj->setTitleFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setTitleFontSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setTitleFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Button:setScale9Enabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setScale9Enabled'", nullptr);
            return 0;
        }
        cobj->setScale9Enabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setScale9Enabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getTitleRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getTitleRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getTitleRenderer'", nullptr);
            return 0;
        }
        cocos2d::Label* ret = cobj->getTitleRenderer();
        object_to_luaval<cocos2d::Label>(tolua_S, "cc.Label",(cocos2d::Label*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getTitleRenderer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getTitleRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getZoomScale(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getZoomScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getZoomScale'", nullptr);
            return 0;
        }
        double ret = cobj->getZoomScale();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getZoomScale",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getZoomScale'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getCapInsetsDisabledRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getCapInsetsDisabledRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getCapInsetsDisabledRenderer'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getCapInsetsDisabledRenderer();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getCapInsetsDisabledRenderer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getCapInsetsDisabledRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setTitleColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setTitleColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "ccui.Button:setTitleColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setTitleColor'", nullptr);
            return 0;
        }
        cobj->setTitleColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setTitleColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setTitleColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setCapInsetsDisabledRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setCapInsetsDisabledRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Button:setCapInsetsDisabledRenderer");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setCapInsetsDisabledRenderer'", nullptr);
            return 0;
        }
        cobj->setCapInsetsDisabledRenderer(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setCapInsetsDisabledRenderer",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setCapInsetsDisabledRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Button:setCapInsets");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setCapInsets'", nullptr);
            return 0;
        }
        cobj->setCapInsets(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setCapInsets",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_loadTextureDisabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_loadTextureDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTextureDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTextureDisabled'", nullptr);
            return 0;
        }
        cobj->loadTextureDisabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTextureDisabled");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Button:loadTextureDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTextureDisabled'", nullptr);
            return 0;
        }
        cobj->loadTextureDisabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:loadTextureDisabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_loadTextureDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setTitleText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setTitleText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:setTitleText");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setTitleText'", nullptr);
            return 0;
        }
        cobj->setTitleText(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setTitleText",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setTitleText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setCapInsetsNormalRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setCapInsetsNormalRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Button:setCapInsetsNormalRenderer");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setCapInsetsNormalRenderer'", nullptr);
            return 0;
        }
        cobj->setCapInsetsNormalRenderer(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setCapInsetsNormalRenderer",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setCapInsetsNormalRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_loadTexturePressed(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_loadTexturePressed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTexturePressed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTexturePressed'", nullptr);
            return 0;
        }
        cobj->loadTexturePressed(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTexturePressed");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Button:loadTexturePressed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTexturePressed'", nullptr);
            return 0;
        }
        cobj->loadTexturePressed(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:loadTexturePressed",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_loadTexturePressed'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setTitleFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setTitleFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:setTitleFontName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setTitleFontName'", nullptr);
            return 0;
        }
        cobj->setTitleFontName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setTitleFontName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setTitleFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getCapInsetsNormalRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getCapInsetsNormalRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getCapInsetsNormalRenderer'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getCapInsetsNormalRenderer();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getCapInsetsNormalRenderer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getCapInsetsNormalRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getCapInsetsPressedRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getCapInsetsPressedRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getCapInsetsPressedRenderer'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getCapInsetsPressedRenderer();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getCapInsetsPressedRenderer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getCapInsetsPressedRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_loadTextures(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_loadTextures'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        std::string arg0;
        std::string arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Button:loadTextures");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTextures'", nullptr);
            return 0;
        }
        cobj->loadTextures(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Button:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.Button:loadTextures");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTextures'", nullptr);
            return 0;
        }
        cobj->loadTextures(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 4) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;
        cocos2d::ui::Widget::TextureResType arg3;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Button:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.Button:loadTextures");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "ccui.Button:loadTextures");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTextures'", nullptr);
            return 0;
        }
        cobj->loadTextures(arg0, arg1, arg2, arg3);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:loadTextures",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_loadTextures'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_isScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_isScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_isScale9Enabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isScale9Enabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:isScale9Enabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_isScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_loadTextureNormal(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_loadTextureNormal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTextureNormal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTextureNormal'", nullptr);
            return 0;
        }
        cobj->loadTextureNormal(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:loadTextureNormal");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Button:loadTextureNormal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_loadTextureNormal'", nullptr);
            return 0;
        }
        cobj->loadTextureNormal(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:loadTextureNormal",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_loadTextureNormal'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setCapInsetsPressedRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setCapInsetsPressedRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Button:setCapInsetsPressedRenderer");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setCapInsetsPressedRenderer'", nullptr);
            return 0;
        }
        cobj->setCapInsetsPressedRenderer(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setCapInsetsPressedRenderer",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setCapInsetsPressedRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getTitleFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getTitleFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getTitleFontSize'", nullptr);
            return 0;
        }
        double ret = cobj->getTitleFontSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getTitleFontSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getTitleFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getTitleFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getTitleFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getTitleFontName'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getTitleFontName();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getTitleFontName",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getTitleFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_getTitleColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_getTitleColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_getTitleColor'", nullptr);
            return 0;
        }
        const cocos2d::Color3B& ret = cobj->getTitleColor();
        color3b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:getTitleColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_getTitleColor'.",&tolua_err);
#endif

    return 0;
}

int lua_cocos2dx_ui_Button_setTitleOffset(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setTitleOffset'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 2)
    {
        lua_Number x = lua_tonumber(tolua_S, 2);
        lua_Number y = lua_tonumber(tolua_S, 3);
        cobj->setTitleOffset(x, y);
        return 0;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setTitleOffset",argc, 2);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setTitleOffset'.",&tolua_err);
#endif
    
    return 0;
}

int lua_cocos2dx_ui_Button_setPressedActionEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setPressedActionEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Button:setPressedActionEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setPressedActionEnabled'", nullptr);
            return 0;
        }
        cobj->setPressedActionEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setPressedActionEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setPressedActionEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_setZoomScale(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Button*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Button_setZoomScale'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.Button:setZoomScale");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_setZoomScale'", nullptr);
            return 0;
        }
        cobj->setZoomScale(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:setZoomScale",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_setZoomScale'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Button_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:create");
            if (!ok) { break; }
            cocos2d::ui::Button* ret = cocos2d::ui::Button::create(arg0);
            object_to_luaval<cocos2d::ui::Button>(tolua_S, "ccui.Button",(cocos2d::ui::Button*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Button:create");
            if (!ok) { break; }
            cocos2d::ui::Button* ret = cocos2d::ui::Button::create(arg0, arg1);
            object_to_luaval<cocos2d::ui::Button>(tolua_S, "ccui.Button",(cocos2d::ui::Button*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 3)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Button:create");
            if (!ok) { break; }
            std::string arg2;
            ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.Button:create");
            if (!ok) { break; }
            cocos2d::ui::Button* ret = cocos2d::ui::Button::create(arg0, arg1, arg2);
            object_to_luaval<cocos2d::ui::Button>(tolua_S, "ccui.Button",(cocos2d::ui::Button*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 4)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Button:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Button:create");
            if (!ok) { break; }
            std::string arg2;
            ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.Button:create");
            if (!ok) { break; }
            cocos2d::ui::Widget::TextureResType arg3;
            ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "ccui.Button:create");
            if (!ok) { break; }
            cocos2d::ui::Button* ret = cocos2d::ui::Button::create(arg0, arg1, arg2, arg3);
            object_to_luaval<cocos2d::ui::Button>(tolua_S, "ccui.Button",(cocos2d::ui::Button*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::Button* ret = cocos2d::ui::Button::create();
            object_to_luaval<cocos2d::ui::Button>(tolua_S, "ccui.Button",(cocos2d::ui::Button*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.Button:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Button_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Button",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::Button::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Button:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Button_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Button* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Button_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::Button();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.Button");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Button:Button",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Button_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_Button_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Button)");
    return 0;
}

int lua_register_cocos2dx_ui_Button(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.Button");
    tolua_cclass(tolua_S,"Button","ccui.Button","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"Button");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_Button_constructor);
        tolua_function(tolua_S,"getTitleText",lua_cocos2dx_ui_Button_getTitleText);
        tolua_function(tolua_S,"setTitleFontSize",lua_cocos2dx_ui_Button_setTitleFontSize);
        tolua_function(tolua_S,"setScale9Enabled",lua_cocos2dx_ui_Button_setScale9Enabled);
        tolua_function(tolua_S,"getTitleRenderer",lua_cocos2dx_ui_Button_getTitleRenderer);
        tolua_function(tolua_S,"getZoomScale",lua_cocos2dx_ui_Button_getZoomScale);
        tolua_function(tolua_S,"getCapInsetsDisabledRenderer",lua_cocos2dx_ui_Button_getCapInsetsDisabledRenderer);
        tolua_function(tolua_S,"setTitleColor",lua_cocos2dx_ui_Button_setTitleColor);
        tolua_function(tolua_S,"setCapInsetsDisabledRenderer",lua_cocos2dx_ui_Button_setCapInsetsDisabledRenderer);
        tolua_function(tolua_S,"setCapInsets",lua_cocos2dx_ui_Button_setCapInsets);
        tolua_function(tolua_S,"loadTextureDisabled",lua_cocos2dx_ui_Button_loadTextureDisabled);
        tolua_function(tolua_S,"setTitleText",lua_cocos2dx_ui_Button_setTitleText);
        tolua_function(tolua_S,"setCapInsetsNormalRenderer",lua_cocos2dx_ui_Button_setCapInsetsNormalRenderer);
        tolua_function(tolua_S,"loadTexturePressed",lua_cocos2dx_ui_Button_loadTexturePressed);
        tolua_function(tolua_S,"setTitleFontName",lua_cocos2dx_ui_Button_setTitleFontName);
        tolua_function(tolua_S,"getCapInsetsNormalRenderer",lua_cocos2dx_ui_Button_getCapInsetsNormalRenderer);
        tolua_function(tolua_S,"getCapInsetsPressedRenderer",lua_cocos2dx_ui_Button_getCapInsetsPressedRenderer);
        tolua_function(tolua_S,"loadTextures",lua_cocos2dx_ui_Button_loadTextures);
        tolua_function(tolua_S,"isScale9Enabled",lua_cocos2dx_ui_Button_isScale9Enabled);
        tolua_function(tolua_S,"loadTextureNormal",lua_cocos2dx_ui_Button_loadTextureNormal);
        tolua_function(tolua_S,"setCapInsetsPressedRenderer",lua_cocos2dx_ui_Button_setCapInsetsPressedRenderer);
        tolua_function(tolua_S,"getTitleFontSize",lua_cocos2dx_ui_Button_getTitleFontSize);
        tolua_function(tolua_S,"getTitleFontName",lua_cocos2dx_ui_Button_getTitleFontName);
        tolua_function(tolua_S,"getTitleColor",lua_cocos2dx_ui_Button_getTitleColor);
        tolua_function(tolua_S,"setTitleOffset",lua_cocos2dx_ui_Button_setTitleOffset);
        tolua_function(tolua_S,"setPressedActionEnabled",lua_cocos2dx_ui_Button_setPressedActionEnabled);
        tolua_function(tolua_S,"setZoomScale",lua_cocos2dx_ui_Button_setZoomScale);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_Button_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_Button_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::Button).name();
    g_luaType[typeName] = "ccui.Button";
    g_typeCast["Button"] = "ccui.Button";
    return 1;
}

int lua_cocos2dx_ui_CheckBox_loadTextureBackGroundSelected(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundSelected'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureBackGroundSelected");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundSelected'", nullptr);
            return 0;
        }
        cobj->loadTextureBackGroundSelected(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureBackGroundSelected");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.CheckBox:loadTextureBackGroundSelected");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundSelected'", nullptr);
            return 0;
        }
        cobj->loadTextureBackGroundSelected(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:loadTextureBackGroundSelected",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundSelected'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_loadTextureBackGroundDisabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureBackGroundDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundDisabled'", nullptr);
            return 0;
        }
        cobj->loadTextureBackGroundDisabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureBackGroundDisabled");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.CheckBox:loadTextureBackGroundDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundDisabled'", nullptr);
            return 0;
        }
        cobj->loadTextureBackGroundDisabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:loadTextureBackGroundDisabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGroundDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_setSelected(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_setSelected'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.CheckBox:setSelected");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_setSelected'", nullptr);
            return 0;
        }
        cobj->setSelected(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:setSelected",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_setSelected'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_addEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_addEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, cocos2d::ui::CheckBox::EventType)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_addEventListener'", nullptr);
            return 0;
        }
        cobj->addEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:addEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_addEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_loadTextureFrontCross(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCross'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureFrontCross");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCross'", nullptr);
            return 0;
        }
        cobj->loadTextureFrontCross(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureFrontCross");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.CheckBox:loadTextureFrontCross");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCross'", nullptr);
            return 0;
        }
        cobj->loadTextureFrontCross(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:loadTextureFrontCross",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCross'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_isSelected(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_isSelected'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_isSelected'", nullptr);
            return 0;
        }
        bool ret = cobj->isSelected();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:isSelected",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_isSelected'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_loadTextures(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_loadTextures'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;
        std::string arg3;
        std::string arg4;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 5,&arg3, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ccui.CheckBox:loadTextures");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextures'", nullptr);
            return 0;
        }
        cobj->loadTextures(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 6) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;
        std::string arg3;
        std::string arg4;
        cocos2d::ui::Widget::TextureResType arg5;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 5,&arg3, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ccui.CheckBox:loadTextures");

        ok &= luaval_to_int32(tolua_S, 7,(int *)&arg5, "ccui.CheckBox:loadTextures");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextures'", nullptr);
            return 0;
        }
        cobj->loadTextures(arg0, arg1, arg2, arg3, arg4, arg5);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:loadTextures",argc, 5);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_loadTextures'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_loadTextureBackGround(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGround'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureBackGround");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGround'", nullptr);
            return 0;
        }
        cobj->loadTextureBackGround(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureBackGround");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.CheckBox:loadTextureBackGround");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGround'", nullptr);
            return 0;
        }
        cobj->loadTextureBackGround(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:loadTextureBackGround",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_loadTextureBackGround'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_loadTextureFrontCrossDisabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::CheckBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCrossDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureFrontCrossDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCrossDisabled'", nullptr);
            return 0;
        }
        cobj->loadTextureFrontCrossDisabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:loadTextureFrontCrossDisabled");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.CheckBox:loadTextureFrontCrossDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCrossDisabled'", nullptr);
            return 0;
        }
        cobj->loadTextureFrontCrossDisabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:loadTextureFrontCrossDisabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_loadTextureFrontCrossDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_CheckBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 5)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg2;
            ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg3;
            ok &= luaval_to_std_string(tolua_S, 5,&arg3, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg4;
            ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ccui.CheckBox:create");
            if (!ok) { break; }
            cocos2d::ui::CheckBox* ret = cocos2d::ui::CheckBox::create(arg0, arg1, arg2, arg3, arg4);
            object_to_luaval<cocos2d::ui::CheckBox>(tolua_S, "ccui.CheckBox",(cocos2d::ui::CheckBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 6)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg2;
            ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg3;
            ok &= luaval_to_std_string(tolua_S, 5,&arg3, "ccui.CheckBox:create");
            if (!ok) { break; }
            std::string arg4;
            ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ccui.CheckBox:create");
            if (!ok) { break; }
            cocos2d::ui::Widget::TextureResType arg5;
            ok &= luaval_to_int32(tolua_S, 7,(int *)&arg5, "ccui.CheckBox:create");
            if (!ok) { break; }
            cocos2d::ui::CheckBox* ret = cocos2d::ui::CheckBox::create(arg0, arg1, arg2, arg3, arg4, arg5);
            object_to_luaval<cocos2d::ui::CheckBox>(tolua_S, "ccui.CheckBox",(cocos2d::ui::CheckBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::CheckBox* ret = cocos2d::ui::CheckBox::create();
            object_to_luaval<cocos2d::ui::CheckBox>(tolua_S, "ccui.CheckBox",(cocos2d::ui::CheckBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.CheckBox:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_CheckBox_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.CheckBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::CheckBox::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.CheckBox:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_CheckBox_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::CheckBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_CheckBox_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::CheckBox();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.CheckBox");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.CheckBox:CheckBox",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_CheckBox_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_CheckBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (CheckBox)");
    return 0;
}

int lua_register_cocos2dx_ui_CheckBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.CheckBox");
    tolua_cclass(tolua_S,"CheckBox","ccui.CheckBox","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"CheckBox");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_CheckBox_constructor);
        tolua_function(tolua_S,"loadTextureBackGroundSelected",lua_cocos2dx_ui_CheckBox_loadTextureBackGroundSelected);
        tolua_function(tolua_S,"loadTextureBackGroundDisabled",lua_cocos2dx_ui_CheckBox_loadTextureBackGroundDisabled);
        tolua_function(tolua_S,"setSelected",lua_cocos2dx_ui_CheckBox_setSelected);
        tolua_function(tolua_S,"addEventListener",lua_cocos2dx_ui_CheckBox_addEventListener);
        tolua_function(tolua_S,"loadTextureFrontCross",lua_cocos2dx_ui_CheckBox_loadTextureFrontCross);
        tolua_function(tolua_S,"isSelected",lua_cocos2dx_ui_CheckBox_isSelected);
        tolua_function(tolua_S,"loadTextures",lua_cocos2dx_ui_CheckBox_loadTextures);
        tolua_function(tolua_S,"loadTextureBackGround",lua_cocos2dx_ui_CheckBox_loadTextureBackGround);
        tolua_function(tolua_S,"loadTextureFrontCrossDisabled",lua_cocos2dx_ui_CheckBox_loadTextureFrontCrossDisabled);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_CheckBox_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_CheckBox_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::CheckBox).name();
    g_luaType[typeName] = "ccui.CheckBox";
    g_typeCast["CheckBox"] = "ccui.CheckBox";
    return 1;
}

int lua_cocos2dx_ui_ImageView_loadTexture(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ImageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ImageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ImageView_loadTexture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.ImageView:loadTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_loadTexture'", nullptr);
            return 0;
        }
        cobj->loadTexture(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.ImageView:loadTexture");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.ImageView:loadTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_loadTexture'", nullptr);
            return 0;
        }
        cobj->loadTexture(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ImageView:loadTexture",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_loadTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ImageView_setScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ImageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ImageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ImageView_setScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.ImageView:setScale9Enabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_setScale9Enabled'", nullptr);
            return 0;
        }
        cobj->setScale9Enabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ImageView:setScale9Enabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_setScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ImageView_setTextureRect(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ImageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ImageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ImageView_setTextureRect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.ImageView:setTextureRect");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_setTextureRect'", nullptr);
            return 0;
        }
        cobj->setTextureRect(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ImageView:setTextureRect",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_setTextureRect'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ImageView_setCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ImageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ImageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ImageView_setCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.ImageView:setCapInsets");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_setCapInsets'", nullptr);
            return 0;
        }
        cobj->setCapInsets(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ImageView:setCapInsets",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_setCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ImageView_getCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ImageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ImageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ImageView_getCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_getCapInsets'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getCapInsets();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ImageView:getCapInsets",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_getCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ImageView_isScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ImageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ImageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ImageView_isScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_isScale9Enabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isScale9Enabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ImageView:isScale9Enabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_isScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ImageView_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.ImageView:create");
            if (!ok) { break; }
            cocos2d::ui::ImageView* ret = cocos2d::ui::ImageView::create(arg0);
            object_to_luaval<cocos2d::ui::ImageView>(tolua_S, "ccui.ImageView",(cocos2d::ui::ImageView*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.ImageView:create");
            if (!ok) { break; }
            cocos2d::ui::Widget::TextureResType arg1;
            ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.ImageView:create");
            if (!ok) { break; }
            cocos2d::ui::ImageView* ret = cocos2d::ui::ImageView::create(arg0, arg1);
            object_to_luaval<cocos2d::ui::ImageView>(tolua_S, "ccui.ImageView",(cocos2d::ui::ImageView*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::ImageView* ret = cocos2d::ui::ImageView::create();
            object_to_luaval<cocos2d::ui::ImageView>(tolua_S, "ccui.ImageView",(cocos2d::ui::ImageView*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.ImageView:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_ImageView_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.ImageView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::ImageView::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.ImageView:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_ImageView_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ImageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ImageView_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::ImageView();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.ImageView");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ImageView:ImageView",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ImageView_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_ImageView_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ImageView)");
    return 0;
}

int lua_register_cocos2dx_ui_ImageView(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.ImageView");
    tolua_cclass(tolua_S,"ImageView","ccui.ImageView","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"ImageView");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_ImageView_constructor);
        tolua_function(tolua_S,"loadTexture",lua_cocos2dx_ui_ImageView_loadTexture);
        tolua_function(tolua_S,"setScale9Enabled",lua_cocos2dx_ui_ImageView_setScale9Enabled);
        tolua_function(tolua_S,"setTextureRect",lua_cocos2dx_ui_ImageView_setTextureRect);
        tolua_function(tolua_S,"setCapInsets",lua_cocos2dx_ui_ImageView_setCapInsets);
        tolua_function(tolua_S,"getCapInsets",lua_cocos2dx_ui_ImageView_getCapInsets);
        tolua_function(tolua_S,"isScale9Enabled",lua_cocos2dx_ui_ImageView_isScale9Enabled);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_ImageView_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_ImageView_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::ImageView).name();
    g_luaType[typeName] = "ccui.ImageView";
    g_typeCast["ImageView"] = "ccui.ImageView";
    return 1;
}

int lua_cocos2dx_ui_Text_enableShadow(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_enableShadow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_enableShadow'", nullptr);
            return 0;
        }
        cobj->enableShadow();
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 1) 
    {
        cocos2d::Color4B arg0;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.Text:enableShadow");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_enableShadow'", nullptr);
            return 0;
        }
        cobj->enableShadow(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        cocos2d::Color4B arg0;
        cocos2d::Size arg1;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.Text:enableShadow");

        ok &= luaval_to_size(tolua_S, 3, &arg1, "ccui.Text:enableShadow");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_enableShadow'", nullptr);
            return 0;
        }
        cobj->enableShadow(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 3) 
    {
        cocos2d::Color4B arg0;
        cocos2d::Size arg1;
        int arg2;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.Text:enableShadow");

        ok &= luaval_to_size(tolua_S, 3, &arg1, "ccui.Text:enableShadow");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ccui.Text:enableShadow");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_enableShadow'", nullptr);
            return 0;
        }
        cobj->enableShadow(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:enableShadow",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_enableShadow'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getFontSize'", nullptr);
            return 0;
        }
        int ret = cobj->getFontSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getFontSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getString'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getString();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getString",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_disableEffect(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_disableEffect'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_disableEffect'", nullptr);
            return 0;
        }
        cobj->disableEffect();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:disableEffect",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_disableEffect'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getTextColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getTextColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getTextColor'", nullptr);
            return 0;
        }
        const cocos2d::Color4B& ret = cobj->getTextColor();
        color4b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getTextColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getTextColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setTextVerticalAlignment(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setTextVerticalAlignment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::TextVAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Text:setTextVerticalAlignment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setTextVerticalAlignment'", nullptr);
            return 0;
        }
        cobj->setTextVerticalAlignment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setTextVerticalAlignment",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setTextVerticalAlignment'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Text:setFontName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setFontName'", nullptr);
            return 0;
        }
        cobj->setFontName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setFontName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setTouchScaleChangeEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setTouchScaleChangeEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Text:setTouchScaleChangeEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setTouchScaleChangeEnabled'", nullptr);
            return 0;
        }
        cobj->setTouchScaleChangeEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setTouchScaleChangeEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setTouchScaleChangeEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Text:setString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setString'", nullptr);
            return 0;
        }
        cobj->setString(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setString",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_isTouchScaleChangeEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_isTouchScaleChangeEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_isTouchScaleChangeEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isTouchScaleChangeEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:isTouchScaleChangeEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_isTouchScaleChangeEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getFontName'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getFontName();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getFontName",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setTextAreaSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setTextAreaSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.Text:setTextAreaSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setTextAreaSize'", nullptr);
            return 0;
        }
        cobj->setTextAreaSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setTextAreaSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setTextAreaSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getStringLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getStringLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getStringLength'", nullptr);
            return 0;
        }
        ssize_t ret = cobj->getStringLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getStringLength",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getStringLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_enableOutline(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_enableOutline'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color4B arg0;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.Text:enableOutline");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_enableOutline'", nullptr);
            return 0;
        }
        cobj->enableOutline(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        cocos2d::Color4B arg0;
        int arg1;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.Text:enableOutline");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Text:enableOutline");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_enableOutline'", nullptr);
            return 0;
        }
        cobj->enableOutline(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:enableOutline",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_enableOutline'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getType'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getType();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getTextHorizontalAlignment(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getTextHorizontalAlignment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getTextHorizontalAlignment'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getTextHorizontalAlignment();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getTextHorizontalAlignment",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getTextHorizontalAlignment'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Text:setFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setFontSize'", nullptr);
            return 0;
        }
        cobj->setFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setFontSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setTextColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setTextColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color4B arg0;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.Text:setTextColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setTextColor'", nullptr);
            return 0;
        }
        cobj->setTextColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setTextColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setTextColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_enableGlow(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_enableGlow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color4B arg0;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.Text:enableGlow");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_enableGlow'", nullptr);
            return 0;
        }
        cobj->enableGlow(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:enableGlow",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_enableGlow'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getTextVerticalAlignment(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getTextVerticalAlignment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getTextVerticalAlignment'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getTextVerticalAlignment();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getTextVerticalAlignment",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getTextVerticalAlignment'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_getTextAreaSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_getTextAreaSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_getTextAreaSize'", nullptr);
            return 0;
        }
        const cocos2d::Size& ret = cobj->getTextAreaSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:getTextAreaSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_getTextAreaSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_setTextHorizontalAlignment(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Text*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Text_setTextHorizontalAlignment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::TextHAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Text:setTextHorizontalAlignment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_setTextHorizontalAlignment'", nullptr);
            return 0;
        }
        cobj->setTextHorizontalAlignment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:setTextHorizontalAlignment",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_setTextHorizontalAlignment'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Text_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 3)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Text:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Text:create");
            if (!ok) { break; }
            int arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ccui.Text:create");
            if (!ok) { break; }
            cocos2d::ui::Text* ret = cocos2d::ui::Text::create(arg0, arg1, arg2);
            object_to_luaval<cocos2d::ui::Text>(tolua_S, "ccui.Text",(cocos2d::ui::Text*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::Text* ret = cocos2d::ui::Text::create();
            object_to_luaval<cocos2d::ui::Text>(tolua_S, "ccui.Text",(cocos2d::ui::Text*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.Text:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Text_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Text",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::Text::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Text:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Text_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Text* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Text_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::Text();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.Text");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Text:Text",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Text_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_Text_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Text)");
    return 0;
}

int lua_register_cocos2dx_ui_Text(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.Text");
    tolua_cclass(tolua_S,"Text","ccui.Text","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"Text");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_Text_constructor);
        tolua_function(tolua_S,"enableShadow",lua_cocos2dx_ui_Text_enableShadow);
        tolua_function(tolua_S,"getFontSize",lua_cocos2dx_ui_Text_getFontSize);
        tolua_function(tolua_S,"getString",lua_cocos2dx_ui_Text_getString);
        tolua_function(tolua_S,"disableEffect",lua_cocos2dx_ui_Text_disableEffect);
        tolua_function(tolua_S,"getTextColor",lua_cocos2dx_ui_Text_getTextColor);
        tolua_function(tolua_S,"setTextVerticalAlignment",lua_cocos2dx_ui_Text_setTextVerticalAlignment);
        tolua_function(tolua_S,"setFontName",lua_cocos2dx_ui_Text_setFontName);
        tolua_function(tolua_S,"setTouchScaleChangeEnabled",lua_cocos2dx_ui_Text_setTouchScaleChangeEnabled);
        tolua_function(tolua_S,"setString",lua_cocos2dx_ui_Text_setString);
        tolua_function(tolua_S,"isTouchScaleChangeEnabled",lua_cocos2dx_ui_Text_isTouchScaleChangeEnabled);
        tolua_function(tolua_S,"getFontName",lua_cocos2dx_ui_Text_getFontName);
        tolua_function(tolua_S,"setTextAreaSize",lua_cocos2dx_ui_Text_setTextAreaSize);
        tolua_function(tolua_S,"getStringLength",lua_cocos2dx_ui_Text_getStringLength);
        tolua_function(tolua_S,"enableOutline",lua_cocos2dx_ui_Text_enableOutline);
        tolua_function(tolua_S,"getType",lua_cocos2dx_ui_Text_getType);
        tolua_function(tolua_S,"getTextHorizontalAlignment",lua_cocos2dx_ui_Text_getTextHorizontalAlignment);
        tolua_function(tolua_S,"setFontSize",lua_cocos2dx_ui_Text_setFontSize);
        tolua_function(tolua_S,"setTextColor",lua_cocos2dx_ui_Text_setTextColor);
        tolua_function(tolua_S,"enableGlow",lua_cocos2dx_ui_Text_enableGlow);
        tolua_function(tolua_S,"getTextVerticalAlignment",lua_cocos2dx_ui_Text_getTextVerticalAlignment);
        tolua_function(tolua_S,"getTextAreaSize",lua_cocos2dx_ui_Text_getTextAreaSize);
        tolua_function(tolua_S,"setTextHorizontalAlignment",lua_cocos2dx_ui_Text_setTextHorizontalAlignment);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_Text_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_Text_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::Text).name();
    g_luaType[typeName] = "ccui.Text";
    g_typeCast["Text"] = "ccui.Text";
    return 1;
}

int lua_cocos2dx_ui_TextAtlas_getStringLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextAtlas* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextAtlas",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextAtlas*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextAtlas_getStringLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextAtlas_getStringLength'", nullptr);
            return 0;
        }
        ssize_t ret = cobj->getStringLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextAtlas:getStringLength",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_getStringLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextAtlas_getString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextAtlas* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextAtlas",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextAtlas*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextAtlas_getString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextAtlas_getString'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getString();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextAtlas:getString",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_getString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextAtlas_setString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextAtlas* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextAtlas",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextAtlas*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextAtlas_setString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextAtlas:setString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextAtlas_setString'", nullptr);
            return 0;
        }
        cobj->setString(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextAtlas:setString",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_setString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextAtlas_setProperty(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextAtlas* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextAtlas",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextAtlas*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextAtlas_setProperty'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 5) 
    {
        std::string arg0;
        std::string arg1;
        int arg2;
        int arg3;
        std::string arg4;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextAtlas:setProperty");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.TextAtlas:setProperty");

        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ccui.TextAtlas:setProperty");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "ccui.TextAtlas:setProperty");

        ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ccui.TextAtlas:setProperty");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextAtlas_setProperty'", nullptr);
            return 0;
        }
        cobj->setProperty(arg0, arg1, arg2, arg3, arg4);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextAtlas:setProperty",argc, 5);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_setProperty'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextAtlas_adaptRenderers(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextAtlas* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextAtlas",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextAtlas*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextAtlas_adaptRenderers'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextAtlas_adaptRenderers'", nullptr);
            return 0;
        }
        cobj->adaptRenderers();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextAtlas:adaptRenderers",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_adaptRenderers'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextAtlas_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.TextAtlas",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 5)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextAtlas:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.TextAtlas:create");
            if (!ok) { break; }
            int arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ccui.TextAtlas:create");
            if (!ok) { break; }
            int arg3;
            ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "ccui.TextAtlas:create");
            if (!ok) { break; }
            std::string arg4;
            ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ccui.TextAtlas:create");
            if (!ok) { break; }
            cocos2d::ui::TextAtlas* ret = cocos2d::ui::TextAtlas::create(arg0, arg1, arg2, arg3, arg4);
            object_to_luaval<cocos2d::ui::TextAtlas>(tolua_S, "ccui.TextAtlas",(cocos2d::ui::TextAtlas*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::TextAtlas* ret = cocos2d::ui::TextAtlas::create();
            object_to_luaval<cocos2d::ui::TextAtlas>(tolua_S, "ccui.TextAtlas",(cocos2d::ui::TextAtlas*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.TextAtlas:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_TextAtlas_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.TextAtlas",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextAtlas_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::TextAtlas::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.TextAtlas:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_TextAtlas_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextAtlas* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextAtlas_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::TextAtlas();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.TextAtlas");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextAtlas:TextAtlas",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextAtlas_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_TextAtlas_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (TextAtlas)");
    return 0;
}

int lua_register_cocos2dx_ui_TextAtlas(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.TextAtlas");
    tolua_cclass(tolua_S,"TextAtlas","ccui.TextAtlas","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"TextAtlas");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_TextAtlas_constructor);
        tolua_function(tolua_S,"getStringLength",lua_cocos2dx_ui_TextAtlas_getStringLength);
        tolua_function(tolua_S,"getString",lua_cocos2dx_ui_TextAtlas_getString);
        tolua_function(tolua_S,"setString",lua_cocos2dx_ui_TextAtlas_setString);
        tolua_function(tolua_S,"setProperty",lua_cocos2dx_ui_TextAtlas_setProperty);
        tolua_function(tolua_S,"adaptRenderers",lua_cocos2dx_ui_TextAtlas_adaptRenderers);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_TextAtlas_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_TextAtlas_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::TextAtlas).name();
    g_luaType[typeName] = "ccui.TextAtlas";
    g_typeCast["TextAtlas"] = "ccui.TextAtlas";
    return 1;
}

int lua_cocos2dx_ui_LoadingBar_setPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_setPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.LoadingBar:setPercent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_setPercent'", nullptr);
            return 0;
        }
        cobj->setPercent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:setPercent",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_setPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_loadTexture(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_loadTexture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.LoadingBar:loadTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_loadTexture'", nullptr);
            return 0;
        }
        cobj->loadTexture(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.LoadingBar:loadTexture");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.LoadingBar:loadTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_loadTexture'", nullptr);
            return 0;
        }
        cobj->loadTexture(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:loadTexture",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_loadTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_setDirection(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_setDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::LoadingBar::Direction arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.LoadingBar:setDirection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_setDirection'", nullptr);
            return 0;
        }
        cobj->setDirection(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:setDirection",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_setDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_setScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_setScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.LoadingBar:setScale9Enabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_setScale9Enabled'", nullptr);
            return 0;
        }
        cobj->setScale9Enabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:setScale9Enabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_setScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_setCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_setCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.LoadingBar:setCapInsets");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_setCapInsets'", nullptr);
            return 0;
        }
        cobj->setCapInsets(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:setCapInsets",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_setCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_getDirection(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_getDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_getDirection'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getDirection();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:getDirection",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_getDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_getCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_getCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_getCapInsets'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getCapInsets();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:getCapInsets",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_getCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_isScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_isScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_isScale9Enabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isScale9Enabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:isScale9Enabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_isScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_getPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::LoadingBar*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_LoadingBar_getPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_getPercent'", nullptr);
            return 0;
        }
        double ret = cobj->getPercent();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:getPercent",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_getPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_LoadingBar_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.LoadingBar:create");
            if (!ok) { break; }
            cocos2d::ui::LoadingBar* ret = cocos2d::ui::LoadingBar::create(arg0);
            object_to_luaval<cocos2d::ui::LoadingBar>(tolua_S, "ccui.LoadingBar",(cocos2d::ui::LoadingBar*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.LoadingBar:create");
            if (!ok) { break; }
            double arg1;
            ok &= luaval_to_number(tolua_S, 3,&arg1, "ccui.LoadingBar:create");
            if (!ok) { break; }
            cocos2d::ui::LoadingBar* ret = cocos2d::ui::LoadingBar::create(arg0, arg1);
            object_to_luaval<cocos2d::ui::LoadingBar>(tolua_S, "ccui.LoadingBar",(cocos2d::ui::LoadingBar*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::LoadingBar* ret = cocos2d::ui::LoadingBar::create();
            object_to_luaval<cocos2d::ui::LoadingBar>(tolua_S, "ccui.LoadingBar",(cocos2d::ui::LoadingBar*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.LoadingBar:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_LoadingBar_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.LoadingBar",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::LoadingBar::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.LoadingBar:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_LoadingBar_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::LoadingBar* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_LoadingBar_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::LoadingBar();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.LoadingBar");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.LoadingBar:LoadingBar",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_LoadingBar_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_LoadingBar_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (LoadingBar)");
    return 0;
}

int lua_register_cocos2dx_ui_LoadingBar(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.LoadingBar");
    tolua_cclass(tolua_S,"LoadingBar","ccui.LoadingBar","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"LoadingBar");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_LoadingBar_constructor);
        tolua_function(tolua_S,"setPercent",lua_cocos2dx_ui_LoadingBar_setPercent);
        tolua_function(tolua_S,"loadTexture",lua_cocos2dx_ui_LoadingBar_loadTexture);
        tolua_function(tolua_S,"setDirection",lua_cocos2dx_ui_LoadingBar_setDirection);
        tolua_function(tolua_S,"setScale9Enabled",lua_cocos2dx_ui_LoadingBar_setScale9Enabled);
        tolua_function(tolua_S,"setCapInsets",lua_cocos2dx_ui_LoadingBar_setCapInsets);
        tolua_function(tolua_S,"getDirection",lua_cocos2dx_ui_LoadingBar_getDirection);
        tolua_function(tolua_S,"getCapInsets",lua_cocos2dx_ui_LoadingBar_getCapInsets);
        tolua_function(tolua_S,"isScale9Enabled",lua_cocos2dx_ui_LoadingBar_isScale9Enabled);
        tolua_function(tolua_S,"getPercent",lua_cocos2dx_ui_LoadingBar_getPercent);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_LoadingBar_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_LoadingBar_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::LoadingBar).name();
    g_luaType[typeName] = "ccui.LoadingBar";
    g_typeCast["LoadingBar"] = "ccui.LoadingBar";
    return 1;
}

int lua_cocos2dx_ui_ScrollView_scrollToTop(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToTop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToTop");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToTop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToTop'", nullptr);
            return 0;
        }
        cobj->scrollToTop(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToTop",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToTop'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToPercentHorizontal(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentHorizontal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        double arg0;
        double arg1;
        bool arg2;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToPercentHorizontal");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToPercentHorizontal");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.ScrollView:scrollToPercentHorizontal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentHorizontal'", nullptr);
            return 0;
        }
        cobj->scrollToPercentHorizontal(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToPercentHorizontal",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentHorizontal'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_isInertiaScrollEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_isInertiaScrollEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_isInertiaScrollEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isInertiaScrollEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:isInertiaScrollEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_isInertiaScrollEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToPercentBothDirection(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentBothDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        cocos2d::Vec2 arg0;
        double arg1;
        bool arg2;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.ScrollView:scrollToPercentBothDirection");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToPercentBothDirection");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.ScrollView:scrollToPercentBothDirection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentBothDirection'", nullptr);
            return 0;
        }
        cobj->scrollToPercentBothDirection(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToPercentBothDirection",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentBothDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_getDirection(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_getDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_getDirection'", nullptr);
            return 0;
        }
        int ret = (int)cobj->getDirection();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:getDirection",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_getDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToBottomLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToBottomLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToBottomLeft");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToBottomLeft");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToBottomLeft'", nullptr);
            return 0;
        }
        cobj->scrollToBottomLeft(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToBottomLeft",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToBottomLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_getInnerContainer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_getInnerContainer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_getInnerContainer'", nullptr);
            return 0;
        }
        cocos2d::ui::Layout* ret = cobj->getInnerContainer();
        object_to_luaval<cocos2d::ui::Layout>(tolua_S, "ccui.Layout",(cocos2d::ui::Layout*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:getInnerContainer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_getInnerContainer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToBottom(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToBottom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToBottom'", nullptr);
            return 0;
        }
        cobj->jumpToBottom();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToBottom",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToBottom'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_setDirection(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_setDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::ScrollView::Direction arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.ScrollView:setDirection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_setDirection'", nullptr);
            return 0;
        }
        cobj->setDirection(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:setDirection",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_setDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToTopLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToTopLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToTopLeft");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToTopLeft");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToTopLeft'", nullptr);
            return 0;
        }
        cobj->scrollToTopLeft(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToTopLeft",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToTopLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToTopRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToTopRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToTopRight'", nullptr);
            return 0;
        }
        cobj->jumpToTopRight();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToTopRight",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToTopRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToBottomLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToBottomLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToBottomLeft'", nullptr);
            return 0;
        }
        cobj->jumpToBottomLeft();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToBottomLeft",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToBottomLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_setInnerContainerSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_setInnerContainerSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.ScrollView:setInnerContainerSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_setInnerContainerSize'", nullptr);
            return 0;
        }
        cobj->setInnerContainerSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:setInnerContainerSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_setInnerContainerSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_getInnerContainerSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_getInnerContainerSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_getInnerContainerSize'", nullptr);
            return 0;
        }
        const cocos2d::Size& ret = cobj->getInnerContainerSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:getInnerContainerSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_getInnerContainerSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_isBounceEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_isBounceEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_isBounceEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isBounceEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:isBounceEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_isBounceEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToPercentVertical(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentVertical'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:jumpToPercentVertical");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentVertical'", nullptr);
            return 0;
        }
        cobj->jumpToPercentVertical(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToPercentVertical",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentVertical'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_addEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_addEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, cocos2d::ui::ScrollView::EventType)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_addEventListener'", nullptr);
            return 0;
        }
        cobj->addEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:addEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_addEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_setInertiaScrollEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_setInertiaScrollEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.ScrollView:setInertiaScrollEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_setInertiaScrollEnabled'", nullptr);
            return 0;
        }
        cobj->setInertiaScrollEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:setInertiaScrollEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_setInertiaScrollEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToTopLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToTopLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToTopLeft'", nullptr);
            return 0;
        }
        cobj->jumpToTopLeft();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToTopLeft",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToTopLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToPercentHorizontal(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentHorizontal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:jumpToPercentHorizontal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentHorizontal'", nullptr);
            return 0;
        }
        cobj->jumpToPercentHorizontal(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToPercentHorizontal",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentHorizontal'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToBottomRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToBottomRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToBottomRight'", nullptr);
            return 0;
        }
        cobj->jumpToBottomRight();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToBottomRight",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToBottomRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_setBounceEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_setBounceEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.ScrollView:setBounceEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_setBounceEnabled'", nullptr);
            return 0;
        }
        cobj->setBounceEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:setBounceEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_setBounceEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToTop(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToTop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToTop'", nullptr);
            return 0;
        }
        cobj->jumpToTop();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToTop",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToTop'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToLeft");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToLeft");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToLeft'", nullptr);
            return 0;
        }
        cobj->scrollToLeft(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToLeft",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToPercentBothDirection(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentBothDirection'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.ScrollView:jumpToPercentBothDirection");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentBothDirection'", nullptr);
            return 0;
        }
        cobj->jumpToPercentBothDirection(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToPercentBothDirection",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToPercentBothDirection'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToPercentVertical(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentVertical'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        double arg0;
        double arg1;
        bool arg2;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToPercentVertical");

        ok &= luaval_to_number(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToPercentVertical");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.ScrollView:scrollToPercentVertical");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentVertical'", nullptr);
            return 0;
        }
        cobj->scrollToPercentVertical(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToPercentVertical",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToPercentVertical'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToBottom(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToBottom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToBottom");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToBottom");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToBottom'", nullptr);
            return 0;
        }
        cobj->scrollToBottom(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToBottom",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToBottom'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToBottomRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToBottomRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToBottomRight");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToBottomRight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToBottomRight'", nullptr);
            return 0;
        }
        cobj->scrollToBottomRight(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToBottomRight",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToBottomRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToLeft'", nullptr);
            return 0;
        }
        cobj->jumpToLeft();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToLeft",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToRight");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToRight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToRight'", nullptr);
            return 0;
        }
        cobj->scrollToRight(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToRight",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_jumpToRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_jumpToRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_jumpToRight'", nullptr);
            return 0;
        }
        cobj->jumpToRight();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:jumpToRight",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_jumpToRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_scrollToTopRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ScrollView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ScrollView_scrollToTopRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        double arg0;
        bool arg1;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ScrollView:scrollToTopRight");

        ok &= luaval_to_boolean(tolua_S, 3,&arg1, "ccui.ScrollView:scrollToTopRight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_scrollToTopRight'", nullptr);
            return 0;
        }
        cobj->scrollToTopRight(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:scrollToTopRight",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_scrollToTopRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ScrollView_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_create'", nullptr);
            return 0;
        }
        cocos2d::ui::ScrollView* ret = cocos2d::ui::ScrollView::create();
        object_to_luaval<cocos2d::ui::ScrollView>(tolua_S, "ccui.ScrollView",(cocos2d::ui::ScrollView*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.ScrollView:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_ScrollView_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.ScrollView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::ScrollView::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.ScrollView:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_ScrollView_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ScrollView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ScrollView_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::ScrollView();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.ScrollView");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ScrollView:ScrollView",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ScrollView_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_ScrollView_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ScrollView)");
    return 0;
}

int lua_register_cocos2dx_ui_ScrollView(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.ScrollView");
    tolua_cclass(tolua_S,"ScrollView","ccui.ScrollView","ccui.Layout",nullptr);

    tolua_beginmodule(tolua_S,"ScrollView");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_ScrollView_constructor);
        tolua_function(tolua_S,"scrollToTop",lua_cocos2dx_ui_ScrollView_scrollToTop);
        tolua_function(tolua_S,"scrollToPercentHorizontal",lua_cocos2dx_ui_ScrollView_scrollToPercentHorizontal);
        tolua_function(tolua_S,"isInertiaScrollEnabled",lua_cocos2dx_ui_ScrollView_isInertiaScrollEnabled);
        tolua_function(tolua_S,"scrollToPercentBothDirection",lua_cocos2dx_ui_ScrollView_scrollToPercentBothDirection);
        tolua_function(tolua_S,"getDirection",lua_cocos2dx_ui_ScrollView_getDirection);
        tolua_function(tolua_S,"scrollToBottomLeft",lua_cocos2dx_ui_ScrollView_scrollToBottomLeft);
        tolua_function(tolua_S,"getInnerContainer",lua_cocos2dx_ui_ScrollView_getInnerContainer);
        tolua_function(tolua_S,"jumpToBottom",lua_cocos2dx_ui_ScrollView_jumpToBottom);
        tolua_function(tolua_S,"setDirection",lua_cocos2dx_ui_ScrollView_setDirection);
        tolua_function(tolua_S,"scrollToTopLeft",lua_cocos2dx_ui_ScrollView_scrollToTopLeft);
        tolua_function(tolua_S,"jumpToTopRight",lua_cocos2dx_ui_ScrollView_jumpToTopRight);
        tolua_function(tolua_S,"jumpToBottomLeft",lua_cocos2dx_ui_ScrollView_jumpToBottomLeft);
        tolua_function(tolua_S,"setInnerContainerSize",lua_cocos2dx_ui_ScrollView_setInnerContainerSize);
        tolua_function(tolua_S,"getInnerContainerSize",lua_cocos2dx_ui_ScrollView_getInnerContainerSize);
        tolua_function(tolua_S,"isBounceEnabled",lua_cocos2dx_ui_ScrollView_isBounceEnabled);
        tolua_function(tolua_S,"jumpToPercentVertical",lua_cocos2dx_ui_ScrollView_jumpToPercentVertical);
        tolua_function(tolua_S,"addEventListener",lua_cocos2dx_ui_ScrollView_addEventListener);
        tolua_function(tolua_S,"setInertiaScrollEnabled",lua_cocos2dx_ui_ScrollView_setInertiaScrollEnabled);
        tolua_function(tolua_S,"jumpToTopLeft",lua_cocos2dx_ui_ScrollView_jumpToTopLeft);
        tolua_function(tolua_S,"jumpToPercentHorizontal",lua_cocos2dx_ui_ScrollView_jumpToPercentHorizontal);
        tolua_function(tolua_S,"jumpToBottomRight",lua_cocos2dx_ui_ScrollView_jumpToBottomRight);
        tolua_function(tolua_S,"setBounceEnabled",lua_cocos2dx_ui_ScrollView_setBounceEnabled);
        tolua_function(tolua_S,"jumpToTop",lua_cocos2dx_ui_ScrollView_jumpToTop);
        tolua_function(tolua_S,"scrollToLeft",lua_cocos2dx_ui_ScrollView_scrollToLeft);
        tolua_function(tolua_S,"jumpToPercentBothDirection",lua_cocos2dx_ui_ScrollView_jumpToPercentBothDirection);
        tolua_function(tolua_S,"scrollToPercentVertical",lua_cocos2dx_ui_ScrollView_scrollToPercentVertical);
        tolua_function(tolua_S,"scrollToBottom",lua_cocos2dx_ui_ScrollView_scrollToBottom);
        tolua_function(tolua_S,"scrollToBottomRight",lua_cocos2dx_ui_ScrollView_scrollToBottomRight);
        tolua_function(tolua_S,"jumpToLeft",lua_cocos2dx_ui_ScrollView_jumpToLeft);
        tolua_function(tolua_S,"scrollToRight",lua_cocos2dx_ui_ScrollView_scrollToRight);
        tolua_function(tolua_S,"jumpToRight",lua_cocos2dx_ui_ScrollView_jumpToRight);
        tolua_function(tolua_S,"scrollToTopRight",lua_cocos2dx_ui_ScrollView_scrollToTopRight);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_ScrollView_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_ScrollView_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::ScrollView).name();
    g_luaType[typeName] = "ccui.ScrollView";
    g_typeCast["ScrollView"] = "ccui.ScrollView";
    return 1;
}

int lua_cocos2dx_ui_ListView_getIndex(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_getIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Widget* arg0;

        ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_getIndex'", nullptr);
            return 0;
        }
        ssize_t ret = cobj->getIndex(arg0);
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:getIndex",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_getIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_removeAllItems(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_removeAllItems'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_removeAllItems'", nullptr);
            return 0;
        }
        cobj->removeAllItems();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:removeAllItems",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_removeAllItems'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_setGravity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_setGravity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::ListView::Gravity arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.ListView:setGravity");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_setGravity'", nullptr);
            return 0;
        }
        cobj->setGravity(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:setGravity",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_setGravity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_pushBackCustomItem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_pushBackCustomItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Widget* arg0;

        ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_pushBackCustomItem'", nullptr);
            return 0;
        }
        cobj->pushBackCustomItem(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:pushBackCustomItem",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_pushBackCustomItem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_getItems(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_getItems'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_getItems'", nullptr);
            return 0;
        }
        cocos2d::Vector<cocos2d::ui::Widget *>& ret = cobj->getItems();
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:getItems",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_getItems'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_removeItem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_removeItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize(tolua_S, 2, &arg0, "ccui.ListView:removeItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_removeItem'", nullptr);
            return 0;
        }
        cobj->removeItem(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:removeItem",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_removeItem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_getCurSelectedIndex(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_getCurSelectedIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_getCurSelectedIndex'", nullptr);
            return 0;
        }
        ssize_t ret = cobj->getCurSelectedIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:getCurSelectedIndex",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_getCurSelectedIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_insertDefaultItem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_insertDefaultItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize(tolua_S, 2, &arg0, "ccui.ListView:insertDefaultItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_insertDefaultItem'", nullptr);
            return 0;
        }
        cobj->insertDefaultItem(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:insertDefaultItem",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_insertDefaultItem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_requestRefreshView(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_requestRefreshView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_requestRefreshView'", nullptr);
            return 0;
        }
        cobj->requestRefreshView();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:requestRefreshView",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_requestRefreshView'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_setItemsMargin(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_setItemsMargin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.ListView:setItemsMargin");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_setItemsMargin'", nullptr);
            return 0;
        }
        cobj->setItemsMargin(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:setItemsMargin",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_setItemsMargin'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_refreshView(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_refreshView'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_refreshView'", nullptr);
            return 0;
        }
        cobj->refreshView();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:refreshView",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_refreshView'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_removeLastItem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_removeLastItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_removeLastItem'", nullptr);
            return 0;
        }
        cobj->removeLastItem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:removeLastItem",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_removeLastItem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_getItemsMargin(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_getItemsMargin'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_getItemsMargin'", nullptr);
            return 0;
        }
        double ret = cobj->getItemsMargin();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:getItemsMargin",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_getItemsMargin'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_addEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_addEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, cocos2d::ui::ListView::EventType)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_addEventListener'", nullptr);
            return 0;
        }
        cobj->addEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:addEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_addEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_getItem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_getItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize(tolua_S, 2, &arg0, "ccui.ListView:getItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_getItem'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cobj->getItem(arg0);
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:getItem",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_getItem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_setItemModel(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_setItemModel'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Widget* arg0;

        ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_setItemModel'", nullptr);
            return 0;
        }
        cobj->setItemModel(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:setItemModel",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_setItemModel'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_doLayout(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_doLayout'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_doLayout'", nullptr);
            return 0;
        }
        cobj->doLayout();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:doLayout",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_doLayout'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_pushBackDefaultItem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_pushBackDefaultItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_pushBackDefaultItem'", nullptr);
            return 0;
        }
        cobj->pushBackDefaultItem();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:pushBackDefaultItem",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_pushBackDefaultItem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_insertCustomItem(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::ListView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_ListView_insertCustomItem'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        cocos2d::ui::Widget* arg0;
        ssize_t arg1;

        ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);

        ok &= luaval_to_ssize(tolua_S, 3, &arg1, "ccui.ListView:insertCustomItem");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_insertCustomItem'", nullptr);
            return 0;
        }
        cobj->insertCustomItem(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:insertCustomItem",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_insertCustomItem'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_ListView_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_create'", nullptr);
            return 0;
        }
        cocos2d::ui::ListView* ret = cocos2d::ui::ListView::create();
        object_to_luaval<cocos2d::ui::ListView>(tolua_S, "ccui.ListView",(cocos2d::ui::ListView*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.ListView:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_ListView_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.ListView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::ListView::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.ListView:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_ListView_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::ListView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_ListView_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::ListView();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.ListView");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.ListView:ListView",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_ListView_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_ListView_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (ListView)");
    return 0;
}

int lua_register_cocos2dx_ui_ListView(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.ListView");
    tolua_cclass(tolua_S,"ListView","ccui.ListView","ccui.ScrollView",nullptr);

    tolua_beginmodule(tolua_S,"ListView");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_ListView_constructor);
        tolua_function(tolua_S,"getIndex",lua_cocos2dx_ui_ListView_getIndex);
        tolua_function(tolua_S,"removeAllItems",lua_cocos2dx_ui_ListView_removeAllItems);
        tolua_function(tolua_S,"setGravity",lua_cocos2dx_ui_ListView_setGravity);
        tolua_function(tolua_S,"pushBackCustomItem",lua_cocos2dx_ui_ListView_pushBackCustomItem);
        tolua_function(tolua_S,"getItems",lua_cocos2dx_ui_ListView_getItems);
        tolua_function(tolua_S,"removeItem",lua_cocos2dx_ui_ListView_removeItem);
        tolua_function(tolua_S,"getCurSelectedIndex",lua_cocos2dx_ui_ListView_getCurSelectedIndex);
        tolua_function(tolua_S,"insertDefaultItem",lua_cocos2dx_ui_ListView_insertDefaultItem);
        tolua_function(tolua_S,"requestRefreshView",lua_cocos2dx_ui_ListView_requestRefreshView);
        tolua_function(tolua_S,"setItemsMargin",lua_cocos2dx_ui_ListView_setItemsMargin);
        tolua_function(tolua_S,"refreshView",lua_cocos2dx_ui_ListView_refreshView);
        tolua_function(tolua_S,"removeLastItem",lua_cocos2dx_ui_ListView_removeLastItem);
        tolua_function(tolua_S,"getItemsMargin",lua_cocos2dx_ui_ListView_getItemsMargin);
        tolua_function(tolua_S,"addEventListener",lua_cocos2dx_ui_ListView_addEventListener);
        tolua_function(tolua_S,"getItem",lua_cocos2dx_ui_ListView_getItem);
        tolua_function(tolua_S,"setItemModel",lua_cocos2dx_ui_ListView_setItemModel);
        tolua_function(tolua_S,"doLayout",lua_cocos2dx_ui_ListView_doLayout);
        tolua_function(tolua_S,"pushBackDefaultItem",lua_cocos2dx_ui_ListView_pushBackDefaultItem);
        tolua_function(tolua_S,"insertCustomItem",lua_cocos2dx_ui_ListView_insertCustomItem);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_ListView_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_ListView_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::ListView).name();
    g_luaType[typeName] = "ccui.ListView";
    g_typeCast["ListView"] = "ccui.ListView";
    return 1;
}

int lua_cocos2dx_ui_Slider_setPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_setPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Slider:setPercent");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_setPercent'", nullptr);
            return 0;
        }
        cobj->setPercent(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:setPercent",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_setPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_loadSlidBallTextureDisabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureDisabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTextureDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureDisabled'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTextureDisabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTextureDisabled");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Slider:loadSlidBallTextureDisabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureDisabled'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTextureDisabled(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:loadSlidBallTextureDisabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureDisabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_loadSlidBallTextureNormal(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureNormal'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTextureNormal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureNormal'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTextureNormal(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTextureNormal");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Slider:loadSlidBallTextureNormal");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureNormal'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTextureNormal(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:loadSlidBallTextureNormal",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextureNormal'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_loadBarTexture(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_loadBarTexture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadBarTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadBarTexture'", nullptr);
            return 0;
        }
        cobj->loadBarTexture(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadBarTexture");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Slider:loadBarTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadBarTexture'", nullptr);
            return 0;
        }
        cobj->loadBarTexture(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:loadBarTexture",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_loadBarTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_loadProgressBarTexture(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_loadProgressBarTexture'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadProgressBarTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadProgressBarTexture'", nullptr);
            return 0;
        }
        cobj->loadProgressBarTexture(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadProgressBarTexture");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Slider:loadProgressBarTexture");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadProgressBarTexture'", nullptr);
            return 0;
        }
        cobj->loadProgressBarTexture(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:loadProgressBarTexture",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_loadProgressBarTexture'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_loadSlidBallTextures(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextures'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTextures");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Slider:loadSlidBallTextures");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.Slider:loadSlidBallTextures");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextures'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTextures(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 4) 
    {
        std::string arg0;
        std::string arg1;
        std::string arg2;
        cocos2d::ui::Widget::TextureResType arg3;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTextures");

        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Slider:loadSlidBallTextures");

        ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.Slider:loadSlidBallTextures");

        ok &= luaval_to_int32(tolua_S, 5,(int *)&arg3, "ccui.Slider:loadSlidBallTextures");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextures'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTextures(arg0, arg1, arg2, arg3);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:loadSlidBallTextures",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_loadSlidBallTextures'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_setCapInsetProgressBarRebderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_setCapInsetProgressBarRebderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Slider:setCapInsetProgressBarRebderer");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_setCapInsetProgressBarRebderer'", nullptr);
            return 0;
        }
        cobj->setCapInsetProgressBarRebderer(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:setCapInsetProgressBarRebderer",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_setCapInsetProgressBarRebderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_setCapInsetsBarRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_setCapInsetsBarRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Slider:setCapInsetsBarRenderer");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_setCapInsetsBarRenderer'", nullptr);
            return 0;
        }
        cobj->setCapInsetsBarRenderer(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:setCapInsetsBarRenderer",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_setCapInsetsBarRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_getCapInsetsProgressBarRebderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_getCapInsetsProgressBarRebderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_getCapInsetsProgressBarRebderer'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getCapInsetsProgressBarRebderer();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:getCapInsetsProgressBarRebderer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_getCapInsetsProgressBarRebderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_setScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_setScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Slider:setScale9Enabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_setScale9Enabled'", nullptr);
            return 0;
        }
        cobj->setScale9Enabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:setScale9Enabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_setScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_setCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_setCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Slider:setCapInsets");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_setCapInsets'", nullptr);
            return 0;
        }
        cobj->setCapInsets(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:setCapInsets",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_setCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_addEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_addEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, cocos2d::ui::Slider::EventType)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_addEventListener'", nullptr);
            return 0;
        }
        cobj->addEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:addEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_addEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_loadSlidBallTexturePressed(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_loadSlidBallTexturePressed'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTexturePressed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTexturePressed'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTexturePressed(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        std::string arg0;
        cocos2d::ui::Widget::TextureResType arg1;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Slider:loadSlidBallTexturePressed");

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Slider:loadSlidBallTexturePressed");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_loadSlidBallTexturePressed'", nullptr);
            return 0;
        }
        cobj->loadSlidBallTexturePressed(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:loadSlidBallTexturePressed",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_loadSlidBallTexturePressed'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_isScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_isScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_isScale9Enabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isScale9Enabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:isScale9Enabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_isScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_getCapInsetsBarRenderer(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_getCapInsetsBarRenderer'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_getCapInsetsBarRenderer'", nullptr);
            return 0;
        }
        const cocos2d::Rect& ret = cobj->getCapInsetsBarRenderer();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:getCapInsetsBarRenderer",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_getCapInsetsBarRenderer'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_getPercent(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Slider*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Slider_getPercent'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_getPercent'", nullptr);
            return 0;
        }
        int ret = cobj->getPercent();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:getPercent",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_getPercent'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Slider_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_create'", nullptr);
            return 0;
        }
        cocos2d::ui::Slider* ret = cocos2d::ui::Slider::create();
        object_to_luaval<cocos2d::ui::Slider>(tolua_S, "ccui.Slider",(cocos2d::ui::Slider*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Slider:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Slider_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Slider",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::Slider::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Slider:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Slider_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Slider* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Slider_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::Slider();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.Slider");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Slider:Slider",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Slider_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_Slider_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Slider)");
    return 0;
}

int lua_register_cocos2dx_ui_Slider(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.Slider");
    tolua_cclass(tolua_S,"Slider","ccui.Slider","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"Slider");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_Slider_constructor);
        tolua_function(tolua_S,"setPercent",lua_cocos2dx_ui_Slider_setPercent);
        tolua_function(tolua_S,"loadSlidBallTextureDisabled",lua_cocos2dx_ui_Slider_loadSlidBallTextureDisabled);
        tolua_function(tolua_S,"loadSlidBallTextureNormal",lua_cocos2dx_ui_Slider_loadSlidBallTextureNormal);
        tolua_function(tolua_S,"loadBarTexture",lua_cocos2dx_ui_Slider_loadBarTexture);
        tolua_function(tolua_S,"loadProgressBarTexture",lua_cocos2dx_ui_Slider_loadProgressBarTexture);
        tolua_function(tolua_S,"loadSlidBallTextures",lua_cocos2dx_ui_Slider_loadSlidBallTextures);
        tolua_function(tolua_S,"setCapInsetProgressBarRebderer",lua_cocos2dx_ui_Slider_setCapInsetProgressBarRebderer);
        tolua_function(tolua_S,"setCapInsetsBarRenderer",lua_cocos2dx_ui_Slider_setCapInsetsBarRenderer);
        tolua_function(tolua_S,"getCapInsetsProgressBarRebderer",lua_cocos2dx_ui_Slider_getCapInsetsProgressBarRebderer);
        tolua_function(tolua_S,"setScale9Enabled",lua_cocos2dx_ui_Slider_setScale9Enabled);
        tolua_function(tolua_S,"setCapInsets",lua_cocos2dx_ui_Slider_setCapInsets);
        tolua_function(tolua_S,"addEventListener",lua_cocos2dx_ui_Slider_addEventListener);
        tolua_function(tolua_S,"loadSlidBallTexturePressed",lua_cocos2dx_ui_Slider_loadSlidBallTexturePressed);
        tolua_function(tolua_S,"isScale9Enabled",lua_cocos2dx_ui_Slider_isScale9Enabled);
        tolua_function(tolua_S,"getCapInsetsBarRenderer",lua_cocos2dx_ui_Slider_getCapInsetsBarRenderer);
        tolua_function(tolua_S,"getPercent",lua_cocos2dx_ui_Slider_getPercent);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_Slider_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_Slider_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::Slider).name();
    g_luaType[typeName] = "ccui.Slider";
    g_typeCast["Slider"] = "ccui.Slider";
    return 1;
}

int lua_cocos2dx_ui_TextField_setAttachWithIME(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setAttachWithIME'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.TextField:setAttachWithIME");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setAttachWithIME'", nullptr);
            return 0;
        }
        cobj->setAttachWithIME(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setAttachWithIME",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setAttachWithIME'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getFontSize'", nullptr);
            return 0;
        }
        int ret = cobj->getFontSize();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getFontSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getString'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getString();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getString",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setPasswordStyleText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setPasswordStyleText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ccui.TextField:setPasswordStyleText"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setPasswordStyleText'", nullptr);
            return 0;
        }
        cobj->setPasswordStyleText(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setPasswordStyleText",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setPasswordStyleText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getDeleteBackward(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getDeleteBackward'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getDeleteBackward'", nullptr);
            return 0;
        }
        bool ret = cobj->getDeleteBackward();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getDeleteBackward",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getDeleteBackward'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getPlaceHolder(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getPlaceHolder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getPlaceHolder'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getPlaceHolder();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getPlaceHolder",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getPlaceHolder'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getAttachWithIME(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getAttachWithIME'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getAttachWithIME'", nullptr);
            return 0;
        }
        bool ret = cobj->getAttachWithIME();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getAttachWithIME",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getAttachWithIME'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextField:setFontName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setFontName'", nullptr);
            return 0;
        }
        cobj->setFontName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setFontName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getInsertText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getInsertText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getInsertText'", nullptr);
            return 0;
        }
        bool ret = cobj->getInsertText();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getInsertText",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getInsertText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setInsertText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setInsertText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.TextField:setInsertText");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setInsertText'", nullptr);
            return 0;
        }
        cobj->setInsertText(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setInsertText",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setInsertText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextField:setString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setString'", nullptr);
            return 0;
        }
        cobj->setString(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setString",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getDetachWithIME(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getDetachWithIME'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getDetachWithIME'", nullptr);
            return 0;
        }
        bool ret = cobj->getDetachWithIME();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getDetachWithIME",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getDetachWithIME'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setTextVerticalAlignment(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setTextVerticalAlignment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::TextVAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.TextField:setTextVerticalAlignment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setTextVerticalAlignment'", nullptr);
            return 0;
        }
        cobj->setTextVerticalAlignment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setTextVerticalAlignment",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setTextVerticalAlignment'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_addEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_addEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, cocos2d::ui::TextField::EventType)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_addEventListener'", nullptr);
            return 0;
        }
        cobj->addEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:addEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_addEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_didNotSelectSelf(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_didNotSelectSelf'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_didNotSelectSelf'", nullptr);
            return 0;
        }
        cobj->didNotSelectSelf();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:didNotSelectSelf",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_didNotSelectSelf'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getFontName'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getFontName();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getFontName",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setTextAreaSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setTextAreaSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.TextField:setTextAreaSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setTextAreaSize'", nullptr);
            return 0;
        }
        cobj->setTextAreaSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setTextAreaSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setTextAreaSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_attachWithIME(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_attachWithIME'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_attachWithIME'", nullptr);
            return 0;
        }
        cobj->attachWithIME();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:attachWithIME",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_attachWithIME'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getStringLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getStringLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getStringLength'", nullptr);
            return 0;
        }
        int ret = cobj->getStringLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getStringLength",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getStringLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setPasswordEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setPasswordEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.TextField:setPasswordEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setPasswordEnabled'", nullptr);
            return 0;
        }
        cobj->setPasswordEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setPasswordEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setPasswordEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getPlaceHolderColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getPlaceHolderColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getPlaceHolderColor'", nullptr);
            return 0;
        }
        const cocos2d::Color4B& ret = cobj->getPlaceHolderColor();
        color4b_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getPlaceHolderColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getPlaceHolderColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getPasswordStyleText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getPasswordStyleText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getPasswordStyleText'", nullptr);
            return 0;
        }
        const char* ret = cobj->getPasswordStyleText();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getPasswordStyleText",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getPasswordStyleText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setMaxLengthEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setMaxLengthEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.TextField:setMaxLengthEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setMaxLengthEnabled'", nullptr);
            return 0;
        }
        cobj->setMaxLengthEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setMaxLengthEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setMaxLengthEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_isPasswordEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_isPasswordEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_isPasswordEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isPasswordEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:isPasswordEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_isPasswordEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setDeleteBackward(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setDeleteBackward'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.TextField:setDeleteBackward");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setDeleteBackward'", nullptr);
            return 0;
        }
        cobj->setDeleteBackward(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setDeleteBackward",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setDeleteBackward'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.TextField:setFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setFontSize'", nullptr);
            return 0;
        }
        cobj->setFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setFontSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setPlaceHolder(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setPlaceHolder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextField:setPlaceHolder");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setPlaceHolder'", nullptr);
            return 0;
        }
        cobj->setPlaceHolder(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setPlaceHolder",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setPlaceHolder'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setPlaceHolderColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setPlaceHolderColor'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            cocos2d::Color4B arg0;
            ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.TextField:setPlaceHolderColor");

            if (!ok) { break; }
            cobj->setPlaceHolderColor(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            cocos2d::Color3B arg0;
            ok &= luaval_to_color3b(tolua_S, 2, &arg0, "ccui.TextField:setPlaceHolderColor");

            if (!ok) { break; }
            cobj->setPlaceHolderColor(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.TextField:setPlaceHolderColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setPlaceHolderColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setTextHorizontalAlignment(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setTextHorizontalAlignment'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::TextHAlignment arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.TextField:setTextHorizontalAlignment");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setTextHorizontalAlignment'", nullptr);
            return 0;
        }
        cobj->setTextHorizontalAlignment(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setTextHorizontalAlignment",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setTextHorizontalAlignment'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setTextColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setTextColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color4B arg0;

        ok &=luaval_to_color4b(tolua_S, 2, &arg0, "ccui.TextField:setTextColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setTextColor'", nullptr);
            return 0;
        }
        cobj->setTextColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setTextColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setTextColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getMaxLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getMaxLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getMaxLength'", nullptr);
            return 0;
        }
        int ret = cobj->getMaxLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getMaxLength",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getMaxLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_isMaxLengthEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_isMaxLengthEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_isMaxLengthEnabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isMaxLengthEnabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:isMaxLengthEnabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_isMaxLengthEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setDetachWithIME(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setDetachWithIME'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.TextField:setDetachWithIME");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setDetachWithIME'", nullptr);
            return 0;
        }
        cobj->setDetachWithIME(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setDetachWithIME",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setDetachWithIME'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setTouchAreaEnabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setTouchAreaEnabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.TextField:setTouchAreaEnabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setTouchAreaEnabled'", nullptr);
            return 0;
        }
        cobj->setTouchAreaEnabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setTouchAreaEnabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setTouchAreaEnabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_hitTest(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_hitTest'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.TextField:hitTest");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_hitTest'", nullptr);
            return 0;
        }
        bool ret = cobj->hitTest(arg0);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:hitTest",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_hitTest'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setMaxLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setMaxLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.TextField:setMaxLength");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setMaxLength'", nullptr);
            return 0;
        }
        cobj->setMaxLength(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setMaxLength",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setMaxLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_setTouchSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_setTouchSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.TextField:setTouchSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_setTouchSize'", nullptr);
            return 0;
        }
        cobj->setTouchSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:setTouchSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_setTouchSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_getTouchSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextField*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextField_getTouchSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_getTouchSize'", nullptr);
            return 0;
        }
        cocos2d::Size ret = cobj->getTouchSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:getTouchSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_getTouchSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextField_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 3)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextField:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.TextField:create");
            if (!ok) { break; }
            int arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ccui.TextField:create");
            if (!ok) { break; }
            cocos2d::ui::TextField* ret = cocos2d::ui::TextField::create(arg0, arg1, arg2);
            object_to_luaval<cocos2d::ui::TextField>(tolua_S, "ccui.TextField",(cocos2d::ui::TextField*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::TextField* ret = cocos2d::ui::TextField::create();
            object_to_luaval<cocos2d::ui::TextField>(tolua_S, "ccui.TextField",(cocos2d::ui::TextField*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.TextField:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_TextField_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.TextField",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::TextField::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.TextField:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_TextField_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextField* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextField_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::TextField();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.TextField");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextField:TextField",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextField_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_TextField_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (TextField)");
    return 0;
}

int lua_register_cocos2dx_ui_TextField(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.TextField");
    tolua_cclass(tolua_S,"TextField","ccui.TextField","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"TextField");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_TextField_constructor);
        tolua_function(tolua_S,"setAttachWithIME",lua_cocos2dx_ui_TextField_setAttachWithIME);
        tolua_function(tolua_S,"getFontSize",lua_cocos2dx_ui_TextField_getFontSize);
        tolua_function(tolua_S,"getString",lua_cocos2dx_ui_TextField_getString);
        tolua_function(tolua_S,"setPasswordStyleText",lua_cocos2dx_ui_TextField_setPasswordStyleText);
        tolua_function(tolua_S,"getDeleteBackward",lua_cocos2dx_ui_TextField_getDeleteBackward);
        tolua_function(tolua_S,"getPlaceHolder",lua_cocos2dx_ui_TextField_getPlaceHolder);
        tolua_function(tolua_S,"getAttachWithIME",lua_cocos2dx_ui_TextField_getAttachWithIME);
        tolua_function(tolua_S,"setFontName",lua_cocos2dx_ui_TextField_setFontName);
        tolua_function(tolua_S,"getInsertText",lua_cocos2dx_ui_TextField_getInsertText);
        tolua_function(tolua_S,"setInsertText",lua_cocos2dx_ui_TextField_setInsertText);
        tolua_function(tolua_S,"setString",lua_cocos2dx_ui_TextField_setString);
        tolua_function(tolua_S,"getDetachWithIME",lua_cocos2dx_ui_TextField_getDetachWithIME);
        tolua_function(tolua_S,"setTextVerticalAlignment",lua_cocos2dx_ui_TextField_setTextVerticalAlignment);
        tolua_function(tolua_S,"addEventListener",lua_cocos2dx_ui_TextField_addEventListener);
        tolua_function(tolua_S,"didNotSelectSelf",lua_cocos2dx_ui_TextField_didNotSelectSelf);
        tolua_function(tolua_S,"getFontName",lua_cocos2dx_ui_TextField_getFontName);
        tolua_function(tolua_S,"setTextAreaSize",lua_cocos2dx_ui_TextField_setTextAreaSize);
        tolua_function(tolua_S,"attachWithIME",lua_cocos2dx_ui_TextField_attachWithIME);
        tolua_function(tolua_S,"getStringLength",lua_cocos2dx_ui_TextField_getStringLength);
        tolua_function(tolua_S,"setPasswordEnabled",lua_cocos2dx_ui_TextField_setPasswordEnabled);
        tolua_function(tolua_S,"getPlaceHolderColor",lua_cocos2dx_ui_TextField_getPlaceHolderColor);
        tolua_function(tolua_S,"getPasswordStyleText",lua_cocos2dx_ui_TextField_getPasswordStyleText);
        tolua_function(tolua_S,"setMaxLengthEnabled",lua_cocos2dx_ui_TextField_setMaxLengthEnabled);
        tolua_function(tolua_S,"isPasswordEnabled",lua_cocos2dx_ui_TextField_isPasswordEnabled);
        tolua_function(tolua_S,"setDeleteBackward",lua_cocos2dx_ui_TextField_setDeleteBackward);
        tolua_function(tolua_S,"setFontSize",lua_cocos2dx_ui_TextField_setFontSize);
        tolua_function(tolua_S,"setPlaceHolder",lua_cocos2dx_ui_TextField_setPlaceHolder);
        tolua_function(tolua_S,"setPlaceHolderColor",lua_cocos2dx_ui_TextField_setPlaceHolderColor);
        tolua_function(tolua_S,"setTextHorizontalAlignment",lua_cocos2dx_ui_TextField_setTextHorizontalAlignment);
        tolua_function(tolua_S,"setTextColor",lua_cocos2dx_ui_TextField_setTextColor);
        tolua_function(tolua_S,"getMaxLength",lua_cocos2dx_ui_TextField_getMaxLength);
        tolua_function(tolua_S,"isMaxLengthEnabled",lua_cocos2dx_ui_TextField_isMaxLengthEnabled);
        tolua_function(tolua_S,"setDetachWithIME",lua_cocos2dx_ui_TextField_setDetachWithIME);
        tolua_function(tolua_S,"setTouchAreaEnabled",lua_cocos2dx_ui_TextField_setTouchAreaEnabled);
        tolua_function(tolua_S,"hitTest",lua_cocos2dx_ui_TextField_hitTest);
        tolua_function(tolua_S,"setMaxLength",lua_cocos2dx_ui_TextField_setMaxLength);
        tolua_function(tolua_S,"setTouchSize",lua_cocos2dx_ui_TextField_setTouchSize);
        tolua_function(tolua_S,"getTouchSize",lua_cocos2dx_ui_TextField_getTouchSize);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_TextField_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_TextField_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::TextField).name();
    g_luaType[typeName] = "ccui.TextField";
    g_typeCast["TextField"] = "ccui.TextField";
    return 1;
}

int lua_cocos2dx_ui_TextBMFont_setFntFile(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextBMFont* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextBMFont",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextBMFont*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextBMFont_setFntFile'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextBMFont:setFntFile");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextBMFont_setFntFile'", nullptr);
            return 0;
        }
        cobj->setFntFile(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextBMFont:setFntFile",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextBMFont_setFntFile'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextBMFont_getStringLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextBMFont* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextBMFont",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextBMFont*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextBMFont_getStringLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextBMFont_getStringLength'", nullptr);
            return 0;
        }
        ssize_t ret = cobj->getStringLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextBMFont:getStringLength",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextBMFont_getStringLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextBMFont_setString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextBMFont* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextBMFont",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextBMFont*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextBMFont_setString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::string arg0;

        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextBMFont:setString");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextBMFont_setString'", nullptr);
            return 0;
        }
        cobj->setString(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextBMFont:setString",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextBMFont_setString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextBMFont_getString(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextBMFont* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.TextBMFont",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::TextBMFont*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_TextBMFont_getString'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextBMFont_getString'", nullptr);
            return 0;
        }
        const std::string& ret = cobj->getString();
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextBMFont:getString",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextBMFont_getString'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_TextBMFont_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.TextBMFont",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.TextBMFont:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.TextBMFont:create");
            if (!ok) { break; }
            cocos2d::ui::TextBMFont* ret = cocos2d::ui::TextBMFont::create(arg0, arg1);
            object_to_luaval<cocos2d::ui::TextBMFont>(tolua_S, "ccui.TextBMFont",(cocos2d::ui::TextBMFont*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::TextBMFont* ret = cocos2d::ui::TextBMFont::create();
            object_to_luaval<cocos2d::ui::TextBMFont>(tolua_S, "ccui.TextBMFont",(cocos2d::ui::TextBMFont*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.TextBMFont:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextBMFont_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_TextBMFont_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.TextBMFont",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextBMFont_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::TextBMFont::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.TextBMFont:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextBMFont_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_TextBMFont_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::TextBMFont* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_TextBMFont_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::TextBMFont();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.TextBMFont");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.TextBMFont:TextBMFont",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_TextBMFont_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_TextBMFont_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (TextBMFont)");
    return 0;
}

int lua_register_cocos2dx_ui_TextBMFont(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.TextBMFont");
    tolua_cclass(tolua_S,"TextBMFont","ccui.TextBMFont","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"TextBMFont");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_TextBMFont_constructor);
        tolua_function(tolua_S,"setFntFile",lua_cocos2dx_ui_TextBMFont_setFntFile);
        tolua_function(tolua_S,"getStringLength",lua_cocos2dx_ui_TextBMFont_getStringLength);
        tolua_function(tolua_S,"setString",lua_cocos2dx_ui_TextBMFont_setString);
        tolua_function(tolua_S,"getString",lua_cocos2dx_ui_TextBMFont_getString);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_TextBMFont_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_TextBMFont_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::TextBMFont).name();
    g_luaType[typeName] = "ccui.TextBMFont";
    g_typeCast["TextBMFont"] = "ccui.TextBMFont";
    return 1;
}

int lua_cocos2dx_ui_PageView_getCustomScrollThreshold(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_getCustomScrollThreshold'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_getCustomScrollThreshold'", nullptr);
            return 0;
        }
        double ret = cobj->getCustomScrollThreshold();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:getCustomScrollThreshold",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_getCustomScrollThreshold'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_getCurPageIndex(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_getCurPageIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_getCurPageIndex'", nullptr);
            return 0;
        }
        ssize_t ret = cobj->getCurPageIndex();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:getCurPageIndex",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_getCurPageIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_addWidgetToPage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_addWidgetToPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) 
    {
        cocos2d::ui::Widget* arg0;
        ssize_t arg1;
        bool arg2;

        ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);

        ok &= luaval_to_ssize(tolua_S, 3, &arg1, "ccui.PageView:addWidgetToPage");

        ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.PageView:addWidgetToPage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_addWidgetToPage'", nullptr);
            return 0;
        }
        cobj->addWidgetToPage(arg0, arg1, arg2);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:addWidgetToPage",argc, 3);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_addWidgetToPage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_isUsingCustomScrollThreshold(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_isUsingCustomScrollThreshold'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_isUsingCustomScrollThreshold'", nullptr);
            return 0;
        }
        bool ret = cobj->isUsingCustomScrollThreshold();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:isUsingCustomScrollThreshold",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_isUsingCustomScrollThreshold'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_getPage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_getPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize(tolua_S, 2, &arg0, "ccui.PageView:getPage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_getPage'", nullptr);
            return 0;
        }
        cocos2d::ui::Layout* ret = cobj->getPage(arg0);
        object_to_luaval<cocos2d::ui::Layout>(tolua_S, "ccui.Layout",(cocos2d::ui::Layout*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:getPage",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_getPage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_removePage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_removePage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Layout* arg0;

        ok &= luaval_to_object<cocos2d::ui::Layout>(tolua_S, 2, "ccui.Layout",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_removePage'", nullptr);
            return 0;
        }
        cobj->removePage(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:removePage",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_removePage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_addEventListener(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_addEventListener'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        std::function<void (cocos2d::Ref *, cocos2d::ui::PageView::EventType)> arg0;

        do {
			// Lambda binding for lua is not supported.
			assert(false);
		} while(0)
		;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_addEventListener'", nullptr);
            return 0;
        }
        cobj->addEventListener(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:addEventListener",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_addEventListener'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_setUsingCustomScrollThreshold(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_setUsingCustomScrollThreshold'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.PageView:setUsingCustomScrollThreshold");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_setUsingCustomScrollThreshold'", nullptr);
            return 0;
        }
        cobj->setUsingCustomScrollThreshold(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:setUsingCustomScrollThreshold",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_setUsingCustomScrollThreshold'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_setCustomScrollThreshold(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_setCustomScrollThreshold'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.PageView:setCustomScrollThreshold");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_setCustomScrollThreshold'", nullptr);
            return 0;
        }
        cobj->setCustomScrollThreshold(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:setCustomScrollThreshold",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_setCustomScrollThreshold'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_insertPage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_insertPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        cocos2d::ui::Layout* arg0;
        int arg1;

        ok &= luaval_to_object<cocos2d::ui::Layout>(tolua_S, 2, "ccui.Layout",&arg0);

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.PageView:insertPage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_insertPage'", nullptr);
            return 0;
        }
        cobj->insertPage(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:insertPage",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_insertPage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_scrollToPage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_scrollToPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize(tolua_S, 2, &arg0, "ccui.PageView:scrollToPage");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_scrollToPage'", nullptr);
            return 0;
        }
        cobj->scrollToPage(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:scrollToPage",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_scrollToPage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_removePageAtIndex(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_removePageAtIndex'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        ssize_t arg0;

        ok &= luaval_to_ssize(tolua_S, 2, &arg0, "ccui.PageView:removePageAtIndex");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_removePageAtIndex'", nullptr);
            return 0;
        }
        cobj->removePageAtIndex(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:removePageAtIndex",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_removePageAtIndex'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_getPages(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_getPages'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_getPages'", nullptr);
            return 0;
        }
        cocos2d::Vector<cocos2d::ui::Layout *>& ret = cobj->getPages();
        ccvector_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:getPages",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_getPages'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_removeAllPages(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_removeAllPages'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_removeAllPages'", nullptr);
            return 0;
        }
        cobj->removeAllPages();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:removeAllPages",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_removeAllPages'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_addPage(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::PageView*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_PageView_addPage'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::Layout* arg0;

        ok &= luaval_to_object<cocos2d::ui::Layout>(tolua_S, 2, "ccui.Layout",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_addPage'", nullptr);
            return 0;
        }
        cobj->addPage(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:addPage",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_addPage'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_PageView_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_create'", nullptr);
            return 0;
        }
        cocos2d::ui::PageView* ret = cocos2d::ui::PageView::create();
        object_to_luaval<cocos2d::ui::PageView>(tolua_S, "ccui.PageView",(cocos2d::ui::PageView*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.PageView:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_PageView_createInstance(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.PageView",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_createInstance'", nullptr);
            return 0;
        }
        cocos2d::Ref* ret = cocos2d::ui::PageView::createInstance();
        object_to_luaval<cocos2d::Ref>(tolua_S, "cc.Ref",(cocos2d::Ref*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.PageView:createInstance",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_createInstance'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_PageView_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::PageView* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_PageView_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::PageView();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.PageView");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.PageView:PageView",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_PageView_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_PageView_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (PageView)");
    return 0;
}

int lua_register_cocos2dx_ui_PageView(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.PageView");
    tolua_cclass(tolua_S,"PageView","ccui.PageView","ccui.Layout",nullptr);

    tolua_beginmodule(tolua_S,"PageView");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_PageView_constructor);
        tolua_function(tolua_S,"getCustomScrollThreshold",lua_cocos2dx_ui_PageView_getCustomScrollThreshold);
        tolua_function(tolua_S,"getCurPageIndex",lua_cocos2dx_ui_PageView_getCurPageIndex);
        tolua_function(tolua_S,"addWidgetToPage",lua_cocos2dx_ui_PageView_addWidgetToPage);
        tolua_function(tolua_S,"isUsingCustomScrollThreshold",lua_cocos2dx_ui_PageView_isUsingCustomScrollThreshold);
        tolua_function(tolua_S,"getPage",lua_cocos2dx_ui_PageView_getPage);
        tolua_function(tolua_S,"removePage",lua_cocos2dx_ui_PageView_removePage);
        tolua_function(tolua_S,"addEventListener",lua_cocos2dx_ui_PageView_addEventListener);
        tolua_function(tolua_S,"setUsingCustomScrollThreshold",lua_cocos2dx_ui_PageView_setUsingCustomScrollThreshold);
        tolua_function(tolua_S,"setCustomScrollThreshold",lua_cocos2dx_ui_PageView_setCustomScrollThreshold);
        tolua_function(tolua_S,"insertPage",lua_cocos2dx_ui_PageView_insertPage);
        tolua_function(tolua_S,"scrollToPage",lua_cocos2dx_ui_PageView_scrollToPage);
        tolua_function(tolua_S,"removePageAtIndex",lua_cocos2dx_ui_PageView_removePageAtIndex);
        tolua_function(tolua_S,"getPages",lua_cocos2dx_ui_PageView_getPages);
        tolua_function(tolua_S,"removeAllPages",lua_cocos2dx_ui_PageView_removeAllPages);
        tolua_function(tolua_S,"addPage",lua_cocos2dx_ui_PageView_addPage);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_PageView_create);
        tolua_function(tolua_S,"createInstance", lua_cocos2dx_ui_PageView_createInstance);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::PageView).name();
    g_luaType[typeName] = "ccui.PageView";
    g_typeCast["PageView"] = "ccui.PageView";
    return 1;
}

int lua_cocos2dx_ui_Helper_getSubStringOfUTF8String(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Helper",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 3)
    {
        std::string arg0;
        unsigned long arg1;
        unsigned long arg2;
        ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Helper:getSubStringOfUTF8String");
        ok &= luaval_to_ulong(tolua_S, 3, &arg1, "ccui.Helper:getSubStringOfUTF8String");
        ok &= luaval_to_ulong(tolua_S, 4, &arg2, "ccui.Helper:getSubStringOfUTF8String");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Helper_getSubStringOfUTF8String'", nullptr);
            return 0;
        }
        std::string ret = cocos2d::ui::Helper::getSubStringOfUTF8String(arg0, arg1, arg2);
        tolua_pushcppstring(tolua_S,ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Helper:getSubStringOfUTF8String",argc, 3);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Helper_getSubStringOfUTF8String'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Helper_changeLayoutSystemActiveState(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Helper",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        bool arg0;
        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Helper:changeLayoutSystemActiveState");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Helper_changeLayoutSystemActiveState'", nullptr);
            return 0;
        }
        cocos2d::ui::Helper::changeLayoutSystemActiveState(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Helper:changeLayoutSystemActiveState",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Helper_changeLayoutSystemActiveState'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Helper_seekActionWidgetByActionTag(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Helper",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        cocos2d::ui::Widget* arg0;
        int arg1;
        ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Helper:seekActionWidgetByActionTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Helper_seekActionWidgetByActionTag'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cocos2d::ui::Helper::seekActionWidgetByActionTag(arg0, arg1);
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Helper:seekActionWidgetByActionTag",argc, 2);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Helper_seekActionWidgetByActionTag'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Helper_seekWidgetByName(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Helper",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        cocos2d::ui::Widget *arg0;
        cocos2d::Node *nodeArg0;
        std::string arg1;
        ok = luaval_to_object<cocos2d::Node>(tolua_S, 2, "cc.Node",&nodeArg0);
        if (!ok) {
            ok = luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);
        } else {
            arg0 = static_cast<cocos2d::ui::Widget *>(nodeArg0);
        }
        
        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Helper:seekWidgetByName");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Helper_seekWidgetByName'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cocos2d::ui::Helper::seekWidgetByName(arg0, arg1);
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Helper:seekWidgetByName",argc, 2);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Helper_seekWidgetByName'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Helper_seekWidgetByTag(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Helper",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 2)
    {
        cocos2d::ui::Widget* arg0;
        cocos2d::Node *nodeArg0;
        int arg1;
        ok &= luaval_to_object<cocos2d::Node>(tolua_S, 2, "cc.Node",&nodeArg0);
        if (!ok) {
            ok &= luaval_to_object<cocos2d::ui::Widget>(tolua_S, 2, "ccui.Widget",&arg0);
        } else {
            arg0 = static_cast<cocos2d::ui::Widget *>(nodeArg0);
        }
        
        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.Helper:seekWidgetByTag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Helper_seekWidgetByTag'", nullptr);
            return 0;
        }
        cocos2d::ui::Widget* ret = cocos2d::ui::Helper::seekWidgetByTag(arg0, arg1);
        object_to_luaval<cocos2d::ui::Widget>(tolua_S, "ccui.Widget",(cocos2d::ui::Widget*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Helper:seekWidgetByTag",argc, 2);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Helper_seekWidgetByTag'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Helper_doLayout(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Helper",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 1)
    {
        cocos2d::Node* arg0;
        ok &= luaval_to_object<cocos2d::Node>(tolua_S, 2, "cc.Node",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Helper_doLayout'", nullptr);
            return 0;
        }
        cocos2d::ui::Helper::doLayout(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.Helper:doLayout",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Helper_doLayout'.",&tolua_err);
#endif
    return 0;
}
static int lua_cocos2dx_ui_Helper_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Helper)");
    return 0;
}

int lua_register_cocos2dx_ui_Helper(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.Helper");
    tolua_cclass(tolua_S,"Helper","ccui.Helper","",nullptr);

    tolua_beginmodule(tolua_S,"Helper");
        tolua_function(tolua_S,"getSubStringOfUTF8String", lua_cocos2dx_ui_Helper_getSubStringOfUTF8String);
        tolua_function(tolua_S,"changeLayoutSystemActiveState", lua_cocos2dx_ui_Helper_changeLayoutSystemActiveState);
        tolua_function(tolua_S,"seekActionWidgetByActionTag", lua_cocos2dx_ui_Helper_seekActionWidgetByActionTag);
        tolua_function(tolua_S,"seekWidgetByName", lua_cocos2dx_ui_Helper_seekWidgetByName);
        tolua_function(tolua_S,"seekWidgetByTag", lua_cocos2dx_ui_Helper_seekWidgetByTag);
        tolua_function(tolua_S,"doLayout", lua_cocos2dx_ui_Helper_doLayout);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::Helper).name();
    g_luaType[typeName] = "ccui.Helper";
    g_typeCast["Helper"] = "ccui.Helper";
    return 1;
}

int lua_cocos2dx_ui_RichElement_getType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichElement* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichElement",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RichElement*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichElement_getType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        lua_pushinteger(tolua_S, (int)cobj->getType());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichElement:getType",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichElement_getType'.",&tolua_err);
#endif

    return 0;
}

int lua_cocos2dx_ui_RichElement_getTag(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichElement* cobj = nullptr;
    
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif
    
#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichElement",0,&tolua_err)) goto tolua_lerror;
#endif
    
    cobj = (cocos2d::ui::RichElement*)tolua_tousertype(tolua_S,1,0);
    
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichElement_getTag'", nullptr);
        return 0;
    }
#endif
    
    argc = lua_gettop(tolua_S)-1;
    if (argc == 0)
    {
        lua_pushinteger(tolua_S, cobj->getTag());
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichElement:getTag",argc, 0);
    return 0;
    
#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichElement_getTag'.",&tolua_err);
#endif
    
    return 0;
}

int lua_register_cocos2dx_ui_RichElement(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RichElement");
    tolua_cclass(tolua_S,"RichElement","ccui.RichElement","cc.Ref",nullptr);

    tolua_beginmodule(tolua_S,"RichElement");
        tolua_function(tolua_S,"getType",lua_cocos2dx_ui_RichElement_getType);
        tolua_function(tolua_S,"getTag",lua_cocos2dx_ui_RichElement_getTag);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RichElement).name();
    g_luaType[typeName] = "ccui.RichElement";
    g_typeCast["RichElement"] = "ccui.RichElement";
    return 1;
}

int lua_cocos2dx_ui_RichElementText_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.RichElementText",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 6)
    {
        int arg0;
        cocos2d::Color3B arg1;
        uint16_t arg2;
        std::string arg3;
        std::string arg4;
        double arg5;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.RichElementText:create");
        ok &= luaval_to_color3b(tolua_S, 3, &arg1, "ccui.RichElementText:create");
        ok &= luaval_to_uint16(tolua_S, 4,&arg2, "ccui.RichElementText:create");
        ok &= luaval_to_std_string(tolua_S, 5,&arg3, "ccui.RichElementText:create");
        ok &= luaval_to_std_string(tolua_S, 6,&arg4, "ccui.RichElementText:create");
        ok &= luaval_to_number(tolua_S, 7,&arg5, "ccui.RichElementText:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichElementText_create'", nullptr);
            return 0;
        }
        cocos2d::ui::RichElementText* ret = cocos2d::ui::RichElementText::create(arg0, arg1, arg2, arg3, arg4, arg5);
        object_to_luaval<cocos2d::ui::RichElementText>(tolua_S, "ccui.RichElementText",(cocos2d::ui::RichElementText*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.RichElementText:create",argc, 6);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichElementText_create'.",&tolua_err);
#endif
    return 0;
}


int lua_register_cocos2dx_ui_RichElementText(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RichElementText");
    tolua_cclass(tolua_S,"RichElementText","ccui.RichElementText","ccui.RichElement",nullptr);

    tolua_beginmodule(tolua_S,"RichElementText");
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_RichElementText_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RichElementText).name();
    g_luaType[typeName] = "ccui.RichElementText";
    g_typeCast["RichElementText"] = "ccui.RichElementText";
    return 1;
}

int lua_cocos2dx_ui_RichElementImage_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.RichElementImage",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 4)
    {
        int arg0;
        cocos2d::Color3B arg1;
        uint16_t arg2;
        std::string arg3;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.RichElementImage:create");
        ok &= luaval_to_color3b(tolua_S, 3, &arg1, "ccui.RichElementImage:create");
        ok &= luaval_to_uint16(tolua_S, 4,&arg2, "ccui.RichElementImage:create");
        ok &= luaval_to_std_string(tolua_S, 5,&arg3, "ccui.RichElementImage:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichElementImage_create'", nullptr);
            return 0;
        }
        cocos2d::ui::RichElementImage* ret = cocos2d::ui::RichElementImage::create(arg0, arg1, arg2, arg3);
        object_to_luaval<cocos2d::ui::RichElementImage>(tolua_S, "ccui.RichElementImage",(cocos2d::ui::RichElementImage*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.RichElementImage:create",argc, 4);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichElementImage_create'.",&tolua_err);
#endif
    return 0;
}

int lua_register_cocos2dx_ui_RichElementImage(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RichElementImage");
    tolua_cclass(tolua_S,"RichElementImage","ccui.RichElementImage","ccui.RichElement",nullptr);

    tolua_beginmodule(tolua_S,"RichElementImage");
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_RichElementImage_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RichElementImage).name();
    g_luaType[typeName] = "ccui.RichElementImage";
    g_typeCast["RichElementImage"] = "ccui.RichElementImage";
    return 1;
}

int lua_cocos2dx_ui_RichElementNewLine_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.RichElementNewLine",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;
    if (argc == 3)
    {
        int arg0;
        cocos2d::Color3B arg1;
        uint16_t arg2;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.RichElementNewLine:create");
        ok &= luaval_to_color3b(tolua_S, 3, &arg1, "ccui.RichElementNewLine:create");
        ok &= luaval_to_uint16(tolua_S, 4,&arg2, "ccui.RichElementNewLine:create");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichElementNewLine_create'", nullptr);
            return 0;
        }
        cocos2d::ui::RichElementNewLine* ret = cocos2d::ui::RichElementNewLine::create(arg0, arg1, arg2);
        object_to_luaval<cocos2d::ui::RichElementNewLine>(tolua_S, "ccui.RichElementNewLine",(cocos2d::ui::RichElementNewLine*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.RichElementNewLine:create", argc, 3);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichElementNewLine_create'.",&tolua_err);
#endif
    return 0;
}

int lua_register_cocos2dx_ui_RichElementNewLine(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RichElementNewLine");
    tolua_cclass(tolua_S,"RichElementNewLine","ccui.RichElementNewLine","ccui.RichElement",nullptr);

    tolua_beginmodule(tolua_S,"RichElementNewLine");
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_RichElementNewLine_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RichElementNewLine).name();
    g_luaType[typeName] = "ccui.RichElementNewLine";
    g_typeCast["RichElementNewLine"] = "ccui.RichElementNewLine";
    return 1;
}

int lua_cocos2dx_ui_RichElementCustomNode_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.RichElementCustomNode",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 4)
    {
        int arg0;
        cocos2d::Color3B arg1;
        uint16_t arg2;
        cocos2d::Node* arg3;
        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.RichElementCustomNode:create");
        ok &= luaval_to_color3b(tolua_S, 3, &arg1, "ccui.RichElementCustomNode:create");
        ok &= luaval_to_uint16(tolua_S, 4,&arg2, "ccui.RichElementCustomNode:create");
        ok &= luaval_to_object<cocos2d::Node>(tolua_S, 5, "cc.Node",&arg3);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichElementCustomNode_create'", nullptr);
            return 0;
        }
        cocos2d::ui::RichElementCustomNode* ret = cocos2d::ui::RichElementCustomNode::create(arg0, arg1, arg2, arg3);
        object_to_luaval<cocos2d::ui::RichElementCustomNode>(tolua_S, "ccui.RichElementCustomNode",(cocos2d::ui::RichElementCustomNode*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.RichElementCustomNode:create",argc, 4);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichElementCustomNode_create'.",&tolua_err);
#endif
    return 0;
}

int lua_register_cocos2dx_ui_RichElementCustomNode(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RichElementCustomNode");
    tolua_cclass(tolua_S,"RichElementCustomNode","ccui.RichElementCustomNode","ccui.RichElement",nullptr);

    tolua_beginmodule(tolua_S,"RichElementCustomNode");
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_RichElementCustomNode_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RichElementCustomNode).name();
    g_luaType[typeName] = "ccui.RichElementCustomNode";
    g_typeCast["RichElementCustomNode"] = "ccui.RichElementCustomNode";
    return 1;
}

int lua_cocos2dx_ui_RichText_insertElement(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichText* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RichText*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichText_insertElement'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        cocos2d::ui::RichElement* arg0;
        int arg1;

        ok &= luaval_to_object<cocos2d::ui::RichElement>(tolua_S, 2, "ccui.RichElement",&arg0);

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.RichText:insertElement");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichText_insertElement'", nullptr);
            return 0;
        }
        cobj->insertElement(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichText:insertElement",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_insertElement'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RichText_setAnchorPoint(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichText* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RichText*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichText_setAnchorPoint'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Vec2 arg0;

        ok &= luaval_to_vec2(tolua_S, 2, &arg0, "ccui.RichText:setAnchorPoint");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichText_setAnchorPoint'", nullptr);
            return 0;
        }
        cobj->setAnchorPoint(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichText:setAnchorPoint",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_setAnchorPoint'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RichText_pushBackElement(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichText* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RichText*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichText_pushBackElement'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::RichElement* arg0;

        ok &= luaval_to_object<cocos2d::ui::RichElement>(tolua_S, 2, "ccui.RichElement",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichText_pushBackElement'", nullptr);
            return 0;
        }
        cobj->pushBackElement(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichText:pushBackElement",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_pushBackElement'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RichText_ignoreContentAdaptWithSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichText* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RichText*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichText_ignoreContentAdaptWithSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.RichText:ignoreContentAdaptWithSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichText_ignoreContentAdaptWithSize'", nullptr);
            return 0;
        }
        cobj->ignoreContentAdaptWithSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichText:ignoreContentAdaptWithSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_ignoreContentAdaptWithSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RichText_setVerticalSpace(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichText* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RichText*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichText_setVerticalSpace'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.RichText:setVerticalSpace");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichText_setVerticalSpace'", nullptr);
            return 0;
        }
        cobj->setVerticalSpace(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichText:setVerticalSpace",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_setVerticalSpace'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RichText_formatText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichText* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::RichText*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichText_formatText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichText_formatText'", nullptr);
            return 0;
        }
        cobj->formatText();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RichText:formatText",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_formatText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RichText_removeElement(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RichText* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::RichText*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_RichText_removeElement'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            cocos2d::ui::RichElement* arg0;
            ok &= luaval_to_object<cocos2d::ui::RichElement>(tolua_S, 2, "ccui.RichElement",&arg0);

            if (!ok) { break; }
            cobj->removeElement(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            int arg0;
            ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.RichText:removeElement");

            if (!ok) { break; }
            cobj->removeElement(arg0);
            lua_settop(tolua_S, 1);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.RichText:removeElement",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_removeElement'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_RichText_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.RichText",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S) - 1;

    if (argc == 0)
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RichText_create'", nullptr);
            return 0;
        }
        cocos2d::ui::RichText* ret = cocos2d::ui::RichText::create();
        object_to_luaval<cocos2d::ui::RichText>(tolua_S, "ccui.RichText",(cocos2d::ui::RichText*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "ccui.RichText:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RichText_create'.",&tolua_err);
#endif
    return 0;
}

int lua_register_cocos2dx_ui_RichText(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RichText");
    tolua_cclass(tolua_S,"RichText","ccui.RichText","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"RichText");
        tolua_function(tolua_S,"insertElement",lua_cocos2dx_ui_RichText_insertElement);
        tolua_function(tolua_S,"setAnchorPoint",lua_cocos2dx_ui_RichText_setAnchorPoint);
        tolua_function(tolua_S,"pushBackElement",lua_cocos2dx_ui_RichText_pushBackElement);
        tolua_function(tolua_S,"ignoreContentAdaptWithSize",lua_cocos2dx_ui_RichText_ignoreContentAdaptWithSize);
        tolua_function(tolua_S,"setVerticalSpace",lua_cocos2dx_ui_RichText_setVerticalSpace);
        tolua_function(tolua_S,"formatText",lua_cocos2dx_ui_RichText_formatText);
        tolua_function(tolua_S,"removeElement",lua_cocos2dx_ui_RichText_removeElement);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_RichText_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RichText).name();
    g_luaType[typeName] = "ccui.RichText";
    g_typeCast["RichText"] = "ccui.RichText";
    return 1;
}

int lua_cocos2dx_ui_HBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.HBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            cocos2d::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.HBox:create");
            if (!ok) { break; }
            cocos2d::ui::HBox* ret = cocos2d::ui::HBox::create(arg0);
            object_to_luaval<cocos2d::ui::HBox>(tolua_S, "ccui.HBox",(cocos2d::ui::HBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::HBox* ret = cocos2d::ui::HBox::create();
            object_to_luaval<cocos2d::ui::HBox>(tolua_S, "ccui.HBox",(cocos2d::ui::HBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.HBox:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_HBox_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_HBox_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::HBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_HBox_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::HBox();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.HBox");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.HBox:HBox",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_HBox_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_HBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (HBox)");
    return 0;
}

int lua_register_cocos2dx_ui_HBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.HBox");
    tolua_cclass(tolua_S,"HBox","ccui.HBox","ccui.Layout",nullptr);

    tolua_beginmodule(tolua_S,"HBox");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_HBox_constructor);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_HBox_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::HBox).name();
    g_luaType[typeName] = "ccui.HBox";
    g_typeCast["HBox"] = "ccui.HBox";
    return 1;
}

int lua_cocos2dx_ui_VBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.VBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            cocos2d::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.VBox:create");
            if (!ok) { break; }
            cocos2d::ui::VBox* ret = cocos2d::ui::VBox::create(arg0);
            object_to_luaval<cocos2d::ui::VBox>(tolua_S, "ccui.VBox",(cocos2d::ui::VBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::VBox* ret = cocos2d::ui::VBox::create();
            object_to_luaval<cocos2d::ui::VBox>(tolua_S, "ccui.VBox",(cocos2d::ui::VBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.VBox:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_VBox_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_VBox_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::VBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_VBox_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::VBox();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.VBox");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.VBox:VBox",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_VBox_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_VBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (VBox)");
    return 0;
}

int lua_register_cocos2dx_ui_VBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.VBox");
    tolua_cclass(tolua_S,"VBox","ccui.VBox","ccui.Layout",nullptr);

    tolua_beginmodule(tolua_S,"VBox");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_VBox_constructor);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_VBox_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::VBox).name();
    g_luaType[typeName] = "ccui.VBox";
    g_typeCast["VBox"] = "ccui.VBox";
    return 1;
}

int lua_cocos2dx_ui_RelativeBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.RelativeBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 1)
        {
            cocos2d::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.RelativeBox:create");
            if (!ok) { break; }
            cocos2d::ui::RelativeBox* ret = cocos2d::ui::RelativeBox::create(arg0);
            object_to_luaval<cocos2d::ui::RelativeBox>(tolua_S, "ccui.RelativeBox",(cocos2d::ui::RelativeBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::RelativeBox* ret = cocos2d::ui::RelativeBox::create();
            object_to_luaval<cocos2d::ui::RelativeBox>(tolua_S, "ccui.RelativeBox",(cocos2d::ui::RelativeBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.RelativeBox:create",argc, 0);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeBox_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_RelativeBox_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::RelativeBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_RelativeBox_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::RelativeBox();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.RelativeBox");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.RelativeBox:RelativeBox",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_RelativeBox_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_RelativeBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (RelativeBox)");
    return 0;
}

int lua_register_cocos2dx_ui_RelativeBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.RelativeBox");
    tolua_cclass(tolua_S,"RelativeBox","ccui.RelativeBox","ccui.Layout",nullptr);

    tolua_beginmodule(tolua_S,"RelativeBox");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_RelativeBox_constructor);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_RelativeBox_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::RelativeBox).name();
    g_luaType[typeName] = "ccui.RelativeBox";
    g_typeCast["RelativeBox"] = "ccui.RelativeBox";
    return 1;
}

int lua_cocos2dx_ui_Scale9Sprite_disableCascadeColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_disableCascadeColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_disableCascadeColor'", nullptr);
            return 0;
        }
        cobj->disableCascadeColor();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:disableCascadeColor",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_disableCascadeColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_updateWithSprite(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_updateWithSprite'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 6) {
            cocos2d::Sprite* arg0;
            ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 2, "cc.Sprite",&arg0);

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            cocos2d::Vec2 arg3;
            ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            cocos2d::Size arg4;
            ok &= luaval_to_size(tolua_S, 6, &arg4, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            cocos2d::Rect arg5;
            ok &= luaval_to_rect(tolua_S, 7, &arg5, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            bool ret = cobj->updateWithSprite(arg0, arg1, arg2, arg3, arg4, arg5);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 4) {
            cocos2d::Sprite* arg0;
            ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 2, "cc.Sprite",&arg0);

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            cocos2d::Rect arg3;
            ok &= luaval_to_rect(tolua_S, 5, &arg3, "ccui.Scale9Sprite:updateWithSprite");

            if (!ok) { break; }
            bool ret = cobj->updateWithSprite(arg0, arg1, arg2, arg3);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.Scale9Sprite:updateWithSprite",argc, 4);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_updateWithSprite'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_isFlippedX(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_isFlippedX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_isFlippedX'", nullptr);
            return 0;
        }
        bool ret = cobj->isFlippedX();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:isFlippedX",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_isFlippedX'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setFlippedY(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setFlippedY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Scale9Sprite:setFlippedY");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setFlippedY'", nullptr);
            return 0;
        }
        cobj->setFlippedY(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setFlippedY",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setFlippedY'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setFlippedX(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setFlippedX'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Scale9Sprite:setFlippedX");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setFlippedX'", nullptr);
            return 0;
        }
        cobj->setFlippedX(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setFlippedX",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setFlippedX'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        bool arg0;

        ok &= luaval_to_boolean(tolua_S, 2,&arg0, "ccui.Scale9Sprite:setScale9Enabled");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setScale9Enabled'", nullptr);
            return 0;
        }
        cobj->setScale9Enabled(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setScale9Enabled",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_disableCascadeOpacity(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_disableCascadeOpacity'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_disableCascadeOpacity'", nullptr);
            return 0;
        }
        cobj->disableCascadeOpacity();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:disableCascadeOpacity",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_disableCascadeOpacity'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setInsetBottom(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetBottom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.Scale9Sprite:setInsetBottom");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetBottom'", nullptr);
            return 0;
        }
        cobj->setInsetBottom(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setInsetBottom",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetBottom'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrameName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrameName'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:initWithSpriteFrameName");

            if (!ok) { break; }
            bool ret = cobj->initWithSpriteFrameName(arg0);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:initWithSpriteFrameName");

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:initWithSpriteFrameName");

            if (!ok) { break; }
            bool ret = cobj->initWithSpriteFrameName(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.Scale9Sprite:initWithSpriteFrameName",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrameName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getSprite(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getSprite'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getSprite'", nullptr);
            return 0;
        }
        cocos2d::Sprite* ret = cobj->getSprite();
        object_to_luaval<cocos2d::Sprite>(tolua_S, "cc.Sprite",(cocos2d::Sprite*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getSprite",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getSprite'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setInsetTop(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetTop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.Scale9Sprite:setInsetTop");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetTop'", nullptr);
            return 0;
        }
        cobj->setInsetTop(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setInsetTop",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetTop'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_init(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_init'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 4) {
            cocos2d::Sprite* arg0;
            ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 2, "cc.Sprite",&arg0);

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            cocos2d::Rect arg3;
            ok &= luaval_to_rect(tolua_S, 5, &arg3, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            bool ret = cobj->init(arg0, arg1, arg2, arg3);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 0) {
            bool ret = cobj->init();
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 3) {
            cocos2d::Sprite* arg0;
            ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 2, "cc.Sprite",&arg0);

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            cocos2d::Rect arg2;
            ok &= luaval_to_rect(tolua_S, 4, &arg2, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            bool ret = cobj->init(arg0, arg1, arg2);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 6) {
            cocos2d::Sprite* arg0;
            ok &= luaval_to_object<cocos2d::Sprite>(tolua_S, 2, "cc.Sprite",&arg0);

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            bool arg2;
            ok &= luaval_to_boolean(tolua_S, 4,&arg2, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            cocos2d::Vec2 arg3;
            ok &= luaval_to_vec2(tolua_S, 5, &arg3, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            cocos2d::Size arg4;
            ok &= luaval_to_size(tolua_S, 6, &arg4, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            cocos2d::Rect arg5;
            ok &= luaval_to_rect(tolua_S, 7, &arg5, "ccui.Scale9Sprite:init");

            if (!ok) { break; }
            bool ret = cobj->init(arg0, arg1, arg2, arg3, arg4, arg5);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.Scale9Sprite:init",argc, 6);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_init'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setPreferredSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setPreferredSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Size arg0;

        ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.Scale9Sprite:setPreferredSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setPreferredSize'", nullptr);
            return 0;
        }
        cobj->setPreferredSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setPreferredSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setPreferredSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getInsetRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetRight'", nullptr);
            return 0;
        }
        double ret = cobj->getInsetRight();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getInsetRight",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setSpriteFrame(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setSpriteFrame'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::SpriteFrame* arg0;

        ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0);
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setSpriteFrame'", nullptr);
            return 0;
        }
        cobj->setSpriteFrame(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    if (argc == 2) 
    {
        cocos2d::SpriteFrame* arg0;
        cocos2d::Rect arg1;

        ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0);

        ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:setSpriteFrame");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setSpriteFrame'", nullptr);
            return 0;
        }
        cobj->setSpriteFrame(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setSpriteFrame",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setSpriteFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getInsetBottom(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetBottom'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetBottom'", nullptr);
            return 0;
        }
        double ret = cobj->getInsetBottom();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getInsetBottom",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetBottom'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_resizableSpriteWithCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_resizableSpriteWithCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Scale9Sprite:resizableSpriteWithCapInsets");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_resizableSpriteWithCapInsets'", nullptr);
            return 0;
        }
        cocos2d::ui::Scale9Sprite* ret = cobj->resizableSpriteWithCapInsets(arg0);
        object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:resizableSpriteWithCapInsets",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_resizableSpriteWithCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_isScale9Enabled(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_isScale9Enabled'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_isScale9Enabled'", nullptr);
            return 0;
        }
        bool ret = cobj->isScale9Enabled();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:isScale9Enabled",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_isScale9Enabled'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getCapInsets'", nullptr);
            return 0;
        }
        cocos2d::Rect ret = cobj->getCapInsets();
        rect_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getCapInsets",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getOriginalSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getOriginalSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getOriginalSize'", nullptr);
            return 0;
        }
        cocos2d::Size ret = cobj->getOriginalSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getOriginalSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getOriginalSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_initWithFile(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_initWithFile'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 2) {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            bool ret = cobj->initWithFile(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 3) {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            cocos2d::Rect arg2;
            ok &= luaval_to_rect(tolua_S, 4, &arg2, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            bool ret = cobj->initWithFile(arg0, arg1, arg2);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            cocos2d::Rect arg0;
            ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            bool ret = cobj->initWithFile(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 1) {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:initWithFile");

            if (!ok) { break; }
            bool ret = cobj->initWithFile(arg0);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.Scale9Sprite:initWithFile",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_initWithFile'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getInsetTop(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetTop'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetTop'", nullptr);
            return 0;
        }
        double ret = cobj->getInsetTop();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getInsetTop",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetTop'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setInsetLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.Scale9Sprite:setInsetLeft");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetLeft'", nullptr);
            return 0;
        }
        cobj->setInsetLeft(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setInsetLeft",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrame(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrame'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    do{
        if (argc == 1) {
            cocos2d::SpriteFrame* arg0;
            ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0);

            if (!ok) { break; }
            bool ret = cobj->initWithSpriteFrame(arg0);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    do{
        if (argc == 2) {
            cocos2d::SpriteFrame* arg0;
            ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0);

            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:initWithSpriteFrame");

            if (!ok) { break; }
            bool ret = cobj->initWithSpriteFrame(arg0, arg1);
            tolua_pushboolean(tolua_S,(bool)ret);
            return 1;
        }
    }while(0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.Scale9Sprite:initWithSpriteFrame",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrame'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getPreferredSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getPreferredSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getPreferredSize'", nullptr);
            return 0;
        }
        cocos2d::Size ret = cobj->getPreferredSize();
        size_to_luaval(tolua_S, ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getPreferredSize",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getPreferredSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setCapInsets(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setCapInsets'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Rect arg0;

        ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Scale9Sprite:setCapInsets");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setCapInsets'", nullptr);
            return 0;
        }
        cobj->setCapInsets(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setCapInsets",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setCapInsets'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_isFlippedY(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_isFlippedY'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_isFlippedY'", nullptr);
            return 0;
        }
        bool ret = cobj->isFlippedY();
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:isFlippedY",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_isFlippedY'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getInsetLeft(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetLeft'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetLeft'", nullptr);
            return 0;
        }
        double ret = cobj->getInsetLeft();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getInsetLeft",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getInsetLeft'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setInsetRight(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetRight'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        double arg0;

        ok &= luaval_to_number(tolua_S, 2,&arg0, "ccui.Scale9Sprite:setInsetRight");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetRight'", nullptr);
            return 0;
        }
        cobj->setInsetRight(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setInsetRight",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setInsetRight'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 3)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            cocos2d::Rect arg2;
            ok &= luaval_to_rect(tolua_S, 4, &arg2, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::create(arg0, arg1, arg2);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 0)
        {
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::create();
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            cocos2d::Rect arg0;
            ok &= luaval_to_rect(tolua_S, 2, &arg0, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::create(arg0, arg1);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::create(arg0, arg1);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:create");
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::create(arg0);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.Scale9Sprite:create",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_createWithSpriteFrameName(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:createWithSpriteFrameName");
            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:createWithSpriteFrameName");
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(arg0, arg1);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            std::string arg0;
            ok &= luaval_to_std_string(tolua_S, 2,&arg0, "ccui.Scale9Sprite:createWithSpriteFrameName");
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::createWithSpriteFrameName(arg0);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.Scale9Sprite:createWithSpriteFrameName",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_createWithSpriteFrameName'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_createWithSpriteFrame(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 2)
        {
            cocos2d::SpriteFrame* arg0;
            ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0);
            if (!ok) { break; }
            cocos2d::Rect arg1;
            ok &= luaval_to_rect(tolua_S, 3, &arg1, "ccui.Scale9Sprite:createWithSpriteFrame");
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::createWithSpriteFrame(arg0, arg1);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do 
    {
        if (argc == 1)
        {
            cocos2d::SpriteFrame* arg0;
            ok &= luaval_to_object<cocos2d::SpriteFrame>(tolua_S, 2, "cc.SpriteFrame",&arg0);
            if (!ok) { break; }
            cocos2d::ui::Scale9Sprite* ret = cocos2d::ui::Scale9Sprite::createWithSpriteFrame(arg0);
            object_to_luaval<cocos2d::ui::Scale9Sprite>(tolua_S, "ccui.Scale9Sprite",(cocos2d::ui::Scale9Sprite*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "ccui.Scale9Sprite:createWithSpriteFrame",argc, 1);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_createWithSpriteFrame'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::Scale9Sprite();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.Scale9Sprite");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:Scale9Sprite",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_constructor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_setState(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_setState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) {
        cocos2d::ui::Scale9Sprite::State arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.Scale9Sprite:setState");
        if(!ok) {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_Scale9Sprite_setState'", nullptr);
            return 0;
        }
        cobj->setState(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:setState",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_setState'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_Scale9Sprite_getState(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::Scale9Sprite* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.Scale9Sprite",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::Scale9Sprite*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_Scale9Sprite_getState'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) {
        int ret = (int)cobj->getState();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.Scale9Sprite:getState",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_Scale9Sprite_getState'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_Scale9Sprite_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (Scale9Sprite)");
    return 0;
}

int lua_register_cocos2dx_ui_Scale9Sprite(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.Scale9Sprite");
    tolua_cclass(tolua_S,"Scale9Sprite","ccui.Scale9Sprite","cc.Node",nullptr);

    tolua_beginmodule(tolua_S,"Scale9Sprite");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_Scale9Sprite_constructor);
        tolua_function(tolua_S,"setState",lua_cocos2dx_ui_Scale9Sprite_setState);
        tolua_function(tolua_S,"getState",lua_cocos2dx_ui_Scale9Sprite_getState);
        tolua_function(tolua_S,"disableCascadeColor",lua_cocos2dx_ui_Scale9Sprite_disableCascadeColor);
        tolua_function(tolua_S,"updateWithSprite",lua_cocos2dx_ui_Scale9Sprite_updateWithSprite);
        tolua_function(tolua_S,"isFlippedX",lua_cocos2dx_ui_Scale9Sprite_isFlippedX);
        tolua_function(tolua_S,"setFlippedY",lua_cocos2dx_ui_Scale9Sprite_setFlippedY);
        tolua_function(tolua_S,"setFlippedX",lua_cocos2dx_ui_Scale9Sprite_setFlippedX);
        tolua_function(tolua_S,"setScale9Enabled",lua_cocos2dx_ui_Scale9Sprite_setScale9Enabled);
        tolua_function(tolua_S,"disableCascadeOpacity",lua_cocos2dx_ui_Scale9Sprite_disableCascadeOpacity);
        tolua_function(tolua_S,"setInsetBottom",lua_cocos2dx_ui_Scale9Sprite_setInsetBottom);
        tolua_function(tolua_S,"initWithSpriteFrameName",lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrameName);
        tolua_function(tolua_S,"getSprite",lua_cocos2dx_ui_Scale9Sprite_getSprite);
        tolua_function(tolua_S,"setInsetTop",lua_cocos2dx_ui_Scale9Sprite_setInsetTop);
        tolua_function(tolua_S,"init",lua_cocos2dx_ui_Scale9Sprite_init);
        tolua_function(tolua_S,"setPreferredSize",lua_cocos2dx_ui_Scale9Sprite_setPreferredSize);
        tolua_function(tolua_S,"getInsetRight",lua_cocos2dx_ui_Scale9Sprite_getInsetRight);
        tolua_function(tolua_S,"setSpriteFrame",lua_cocos2dx_ui_Scale9Sprite_setSpriteFrame);
        tolua_function(tolua_S,"getInsetBottom",lua_cocos2dx_ui_Scale9Sprite_getInsetBottom);
        tolua_function(tolua_S,"resizableSpriteWithCapInsets",lua_cocos2dx_ui_Scale9Sprite_resizableSpriteWithCapInsets);
        tolua_function(tolua_S,"isScale9Enabled",lua_cocos2dx_ui_Scale9Sprite_isScale9Enabled);
        tolua_function(tolua_S,"getCapInsets",lua_cocos2dx_ui_Scale9Sprite_getCapInsets);
        tolua_function(tolua_S,"getOriginalSize",lua_cocos2dx_ui_Scale9Sprite_getOriginalSize);
        tolua_function(tolua_S,"initWithFile",lua_cocos2dx_ui_Scale9Sprite_initWithFile);
        tolua_function(tolua_S,"getInsetTop",lua_cocos2dx_ui_Scale9Sprite_getInsetTop);
        tolua_function(tolua_S,"setInsetLeft",lua_cocos2dx_ui_Scale9Sprite_setInsetLeft);
        tolua_function(tolua_S,"initWithSpriteFrame",lua_cocos2dx_ui_Scale9Sprite_initWithSpriteFrame);
        tolua_function(tolua_S,"getPreferredSize",lua_cocos2dx_ui_Scale9Sprite_getPreferredSize);
        tolua_function(tolua_S,"setCapInsets",lua_cocos2dx_ui_Scale9Sprite_setCapInsets);
        tolua_function(tolua_S,"isFlippedY",lua_cocos2dx_ui_Scale9Sprite_isFlippedY);
        tolua_function(tolua_S,"getInsetLeft",lua_cocos2dx_ui_Scale9Sprite_getInsetLeft);
        tolua_function(tolua_S,"setInsetRight",lua_cocos2dx_ui_Scale9Sprite_setInsetRight);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_Scale9Sprite_create);
        tolua_function(tolua_S,"createWithSpriteFrameName", lua_cocos2dx_ui_Scale9Sprite_createWithSpriteFrameName);
        tolua_function(tolua_S,"createWithSpriteFrame", lua_cocos2dx_ui_Scale9Sprite_createWithSpriteFrame);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::Scale9Sprite).name();
    g_luaType[typeName] = "ccui.Scale9Sprite";
    g_typeCast["Scale9Sprite"] = "ccui.Scale9Sprite";
    return 1;
}

int lua_cocos2dx_ui_EditBox_keyboardDidShow(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_keyboardDidShow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::IMEKeyboardNotificationInfo arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR IMEKeyboardNotificationInfo
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_keyboardDidShow'", nullptr);
            return 0;
        }
        cobj->keyboardDidShow(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:keyboardDidShow",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_keyboardDidShow'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_getScriptEditBoxHandler(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_getScriptEditBoxHandler'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_getScriptEditBoxHandler'", nullptr);
            return 0;
        }
        int ret = cobj->getScriptEditBoxHandler();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:getScriptEditBoxHandler",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_getScriptEditBoxHandler'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_getText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_getText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_getText'", nullptr);
            return 0;
        }
        const char* ret = cobj->getText();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:getText",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_getText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_keyboardDidHide(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_keyboardDidHide'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::IMEKeyboardNotificationInfo arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR IMEKeyboardNotificationInfo
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_keyboardDidHide'", nullptr);
            return 0;
        }
        cobj->keyboardDidHide(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:keyboardDidHide",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_keyboardDidHide'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setPlaceholderFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ccui.EditBox:setPlaceholderFontName"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontName'", nullptr);
            return 0;
        }
        cobj->setPlaceholderFontName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setPlaceholderFontName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_getPlaceHolder(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_getPlaceHolder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_getPlaceHolder'", nullptr);
            return 0;
        }
        const char* ret = cobj->getPlaceHolder();
        tolua_pushstring(tolua_S,(const char*)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:getPlaceHolder",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_getPlaceHolder'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setFontName(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setFontName'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ccui.EditBox:setFontName"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setFontName'", nullptr);
            return 0;
        }
        cobj->setFontName(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setFontName",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setFontName'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_registerScriptEditBoxHandler(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_registerScriptEditBoxHandler'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.EditBox:registerScriptEditBoxHandler");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_registerScriptEditBoxHandler'", nullptr);
            return 0;
        }
        cobj->registerScriptEditBoxHandler(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:registerScriptEditBoxHandler",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_registerScriptEditBoxHandler'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setPlaceholderFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.EditBox:setPlaceholderFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontSize'", nullptr);
            return 0;
        }
        cobj->setPlaceholderFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setPlaceholderFontSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setInputMode(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setInputMode'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::EditBox::InputMode arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.EditBox:setInputMode");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setInputMode'", nullptr);
            return 0;
        }
        cobj->setInputMode(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setInputMode",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setInputMode'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_unregisterScriptEditBoxHandler(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_unregisterScriptEditBoxHandler'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_unregisterScriptEditBoxHandler'", nullptr);
            return 0;
        }
        cobj->unregisterScriptEditBoxHandler();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:unregisterScriptEditBoxHandler",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_unregisterScriptEditBoxHandler'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_keyboardWillShow(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_keyboardWillShow'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::IMEKeyboardNotificationInfo arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR IMEKeyboardNotificationInfo
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_keyboardWillShow'", nullptr);
            return 0;
        }
        cobj->keyboardWillShow(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:keyboardWillShow",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_keyboardWillShow'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setPlaceholderFontColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "ccui.EditBox:setPlaceholderFontColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontColor'", nullptr);
            return 0;
        }
        cobj->setPlaceholderFontColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setPlaceholderFontColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFontColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setFontColor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setFontColor'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::Color3B arg0;

        ok &= luaval_to_color3b(tolua_S, 2, &arg0, "ccui.EditBox:setFontColor");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setFontColor'", nullptr);
            return 0;
        }
        cobj->setFontColor(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setFontColor",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setFontColor'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_keyboardWillHide(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_keyboardWillHide'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::IMEKeyboardNotificationInfo arg0;

        #pragma warning NO CONVERSION TO NATIVE FOR IMEKeyboardNotificationInfo
		ok = false;
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_keyboardWillHide'", nullptr);
            return 0;
        }
        cobj->keyboardWillHide(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:keyboardWillHide",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_keyboardWillHide'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_openKeyboard(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_openKeyboard'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) {
        cobj->openKeyboard();
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:openKeyboard",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_openKeyboard'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setPlaceholderFont(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFont'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const char* arg0;
        int arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ccui.EditBox:setPlaceholderFont"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.EditBox:setPlaceholderFont");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFont'", nullptr);
            return 0;
        }
        cobj->setPlaceholderFont(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setPlaceholderFont",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setPlaceholderFont'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setFontSize(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setFontSize'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.EditBox:setFontSize");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setFontSize'", nullptr);
            return 0;
        }
        cobj->setFontSize(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setFontSize",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setFontSize'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_initWithSizeAndBackgroundSprite(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif
    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);
#if COCOS2D_DEBUG >= 1
    if (!cobj)
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_initWithSizeAndBackgroundSprite'", nullptr);
        return 0;
    }
#endif
    argc = lua_gettop(tolua_S)-1;
    if (argc == 3) {
        cocos2d::Size arg0;
        std::string arg1;
        cocos2d::ui::Widget::TextureResType arg2;
        
        ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.EditBox:initWithSizeAndBackgroundSprite");
        ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.EditBox:initWithSizeAndBackgroundSprite");
        ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ccui.EditBox:initWithSizeAndBackgroundSprite");
        if (!ok) {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_initWithSizeAndBackgroundSprite'", nullptr);
            return 0;
        }
        
        bool ret = cobj->initWithSizeAndBackgroundSprite(arg0, arg1, arg2);
        tolua_pushboolean(tolua_S,(bool)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n",  "ccui.EditBox:initWithSizeAndBackgroundSprite",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_initWithSizeAndBackgroundSprite'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setPlaceHolder(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setPlaceHolder'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ccui.EditBox:setPlaceHolder"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setPlaceHolder'", nullptr);
            return 0;
        }
        cobj->setPlaceHolder(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setPlaceHolder",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setPlaceHolder'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setReturnType(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setReturnType'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::EditBox::KeyboardReturnType arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.EditBox:setReturnType");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setReturnType'", nullptr);
            return 0;
        }
        cobj->setReturnType(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setReturnType",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setReturnType'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setInputFlag(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setInputFlag'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        cocos2d::ui::EditBox::InputFlag arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.EditBox:setInputFlag");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setInputFlag'", nullptr);
            return 0;
        }
        cobj->setInputFlag(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setInputFlag",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setInputFlag'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_getMaxLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_getMaxLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_getMaxLength'", nullptr);
            return 0;
        }
        int ret = cobj->getMaxLength();
        tolua_pushnumber(tolua_S,(lua_Number)ret);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:getMaxLength",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_getMaxLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setText(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setText'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        const char* arg0;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ccui.EditBox:setText"); arg0 = arg0_tmp.c_str();
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setText'", nullptr);
            return 0;
        }
        cobj->setText(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setText",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setText'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setMaxLength(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setMaxLength'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 1) 
    {
        int arg0;

        ok &= luaval_to_int32(tolua_S, 2,(int *)&arg0, "ccui.EditBox:setMaxLength");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setMaxLength'", nullptr);
            return 0;
        }
        cobj->setMaxLength(arg0);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setMaxLength",argc, 1);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setMaxLength'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_setFont(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif


#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertype(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    cobj = (cocos2d::ui::EditBox*)tolua_tousertype(tolua_S,1,0);

#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function 'lua_cocos2dx_ui_EditBox_setFont'", nullptr);
        return 0;
    }
#endif

    argc = lua_gettop(tolua_S)-1;
    if (argc == 2) 
    {
        const char* arg0;
        int arg1;

        std::string arg0_tmp; ok &= luaval_to_std_string(tolua_S, 2, &arg0_tmp, "ccui.EditBox:setFont"); arg0 = arg0_tmp.c_str();

        ok &= luaval_to_int32(tolua_S, 3,(int *)&arg1, "ccui.EditBox:setFont");
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_setFont'", nullptr);
            return 0;
        }
        cobj->setFont(arg0, arg1);
        lua_settop(tolua_S, 1);
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:setFont",argc, 2);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_setFont'.",&tolua_err);
#endif

    return 0;
}
int lua_cocos2dx_ui_EditBox_create(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif

#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"ccui.EditBox",0,&tolua_err)) goto tolua_lerror;
#endif

    argc = lua_gettop(tolua_S)-1;

    do 
    {
        if (argc == 3)
        {
            cocos2d::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.EditBox:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.EditBox:create");
            if (!ok) { break; }
            cocos2d::ui::Widget::TextureResType arg2;
            ok &= luaval_to_int32(tolua_S, 4,(int *)&arg2, "ccui.EditBox:create");
            if (!ok) { break; }
            cocos2d::ui::EditBox* ret = cocos2d::ui::EditBox::create(arg0, arg1, arg2);
            object_to_luaval<cocos2d::ui::EditBox>(tolua_S, "ccui.EditBox",(cocos2d::ui::EditBox*)ret);
            return 1;
        }
    } while (0);
    ok  = true;
    do
    {
        if (argc == 5)
        {
            cocos2d::Size arg0;
            ok &= luaval_to_size(tolua_S, 2, &arg0, "ccui.EditBox:create");
            if (!ok) { break; }
            std::string arg1;
            ok &= luaval_to_std_string(tolua_S, 3,&arg1, "ccui.EditBox:create");
            if (!ok) { break; }
            std::string arg2;
            ok &= luaval_to_std_string(tolua_S, 4,&arg2, "ccui.EditBox:create");
            if (!ok) { break; }
            std::string arg3;
            ok &= luaval_to_std_string(tolua_S, 5,&arg3, "ccui.EditBox:create");
            if (!ok) { break; }
            cocos2d::ui::Widget::TextureResType arg4;
            ok &= luaval_to_int32(tolua_S, 6,(int *)&arg4, "ccui.EditBox:create");
            if (!ok) { break; }
            cocos2d::ui::EditBox* ret = cocos2d::ui::EditBox::create(arg0, arg1, arg2, arg3, arg4);
            object_to_luaval<cocos2d::ui::EditBox>(tolua_S, "ccui.EditBox",(cocos2d::ui::EditBox*)ret);
            return 1;
        }
    } while (0);

    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting 3 or 5", "ccui.EditBox:create",argc);
    return 0;
#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_create'.",&tolua_err);
#endif
    return 0;
}
int lua_cocos2dx_ui_EditBox_constructor(lua_State* tolua_S)
{
    int argc = 0;
    cocos2d::ui::EditBox* cobj = nullptr;
    bool ok  = true;

#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
#endif



    argc = lua_gettop(tolua_S)-1;
    if (argc == 0) 
    {
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function 'lua_cocos2dx_ui_EditBox_constructor'", nullptr);
            return 0;
        }
        cobj = new cocos2d::ui::EditBox();
        cobj->autorelease();
        int ID =  (int)cobj->_ID ;
        int* luaID =  &cobj->_luaID ;
        toluafix_pushusertype_ccobject(tolua_S, ID, luaID, (void*)cobj,"ccui.EditBox");
        return 1;
    }
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d \n", "ccui.EditBox:EditBox",argc, 0);
    return 0;

#if COCOS2D_DEBUG >= 1
    tolua_error(tolua_S,"#ferror in function 'lua_cocos2dx_ui_EditBox_constructor'.",&tolua_err);
#endif

    return 0;
}

static int lua_cocos2dx_ui_EditBox_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (EditBox)");
    return 0;
}

int lua_register_cocos2dx_ui_EditBox(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"ccui.EditBox");
    tolua_cclass(tolua_S,"EditBox","ccui.EditBox","ccui.Widget",nullptr);

    tolua_beginmodule(tolua_S,"EditBox");
        tolua_function(tolua_S,"new",lua_cocos2dx_ui_EditBox_constructor);
        tolua_function(tolua_S,"keyboardDidShow",lua_cocos2dx_ui_EditBox_keyboardDidShow);
        tolua_function(tolua_S,"getScriptEditBoxHandler",lua_cocos2dx_ui_EditBox_getScriptEditBoxHandler);
        tolua_function(tolua_S,"getText",lua_cocos2dx_ui_EditBox_getText);
        tolua_function(tolua_S,"keyboardDidHide",lua_cocos2dx_ui_EditBox_keyboardDidHide);
        tolua_function(tolua_S,"setPlaceholderFontName",lua_cocos2dx_ui_EditBox_setPlaceholderFontName);
        tolua_function(tolua_S,"getPlaceHolder",lua_cocos2dx_ui_EditBox_getPlaceHolder);
        tolua_function(tolua_S,"setFontName",lua_cocos2dx_ui_EditBox_setFontName);
        tolua_function(tolua_S,"registerScriptEditBoxHandler",lua_cocos2dx_ui_EditBox_registerScriptEditBoxHandler);
        tolua_function(tolua_S,"setPlaceholderFontSize",lua_cocos2dx_ui_EditBox_setPlaceholderFontSize);
        tolua_function(tolua_S,"setInputMode",lua_cocos2dx_ui_EditBox_setInputMode);
        tolua_function(tolua_S,"unregisterScriptEditBoxHandler",lua_cocos2dx_ui_EditBox_unregisterScriptEditBoxHandler);
        tolua_function(tolua_S,"keyboardWillShow",lua_cocos2dx_ui_EditBox_keyboardWillShow);
        tolua_function(tolua_S,"setPlaceholderFontColor",lua_cocos2dx_ui_EditBox_setPlaceholderFontColor);
        tolua_function(tolua_S,"setFontColor",lua_cocos2dx_ui_EditBox_setFontColor);
        tolua_function(tolua_S,"keyboardWillHide",lua_cocos2dx_ui_EditBox_keyboardWillHide);
        tolua_function(tolua_S,"openKeyboard",lua_cocos2dx_ui_EditBox_openKeyboard);
        tolua_function(tolua_S,"setPlaceholderFont",lua_cocos2dx_ui_EditBox_setPlaceholderFont);
        tolua_function(tolua_S,"setFontSize",lua_cocos2dx_ui_EditBox_setFontSize);
        tolua_function(tolua_S,"initWithSizeAndBackgroundSprite",lua_cocos2dx_ui_EditBox_initWithSizeAndBackgroundSprite);
        tolua_function(tolua_S,"setPlaceHolder",lua_cocos2dx_ui_EditBox_setPlaceHolder);
        tolua_function(tolua_S,"setReturnType",lua_cocos2dx_ui_EditBox_setReturnType);
        tolua_function(tolua_S,"setInputFlag",lua_cocos2dx_ui_EditBox_setInputFlag);
        tolua_function(tolua_S,"getMaxLength",lua_cocos2dx_ui_EditBox_getMaxLength);
        tolua_function(tolua_S,"setText",lua_cocos2dx_ui_EditBox_setText);
        tolua_function(tolua_S,"setMaxLength",lua_cocos2dx_ui_EditBox_setMaxLength);
        tolua_function(tolua_S,"setFont",lua_cocos2dx_ui_EditBox_setFont);
        tolua_function(tolua_S,"create", lua_cocos2dx_ui_EditBox_create);
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(cocos2d::ui::EditBox).name();
    g_luaType[typeName] = "ccui.EditBox";
    g_typeCast["EditBox"] = "ccui.EditBox";
    return 1;
}
TOLUA_API int register_all_cocos2dx_ui(lua_State* tolua_S)
{
	tolua_open(tolua_S);
	
	tolua_module(tolua_S,"ccui",0);
	tolua_beginmodule(tolua_S,"ccui");

	lua_register_cocos2dx_ui_Widget(tolua_S);
	lua_register_cocos2dx_ui_Layout(tolua_S);
	lua_register_cocos2dx_ui_RelativeBox(tolua_S);
	lua_register_cocos2dx_ui_CheckBox(tolua_S);
	lua_register_cocos2dx_ui_TextAtlas(tolua_S);
	lua_register_cocos2dx_ui_TextBMFont(tolua_S);
	lua_register_cocos2dx_ui_LoadingBar(tolua_S);
	lua_register_cocos2dx_ui_TextField(tolua_S);
	lua_register_cocos2dx_ui_Scale9Sprite(tolua_S);
	lua_register_cocos2dx_ui_VBox(tolua_S);
	lua_register_cocos2dx_ui_RichElement(tolua_S);
	lua_register_cocos2dx_ui_RichElementCustomNode(tolua_S);
    lua_register_cocos2dx_ui_RichElementNewLine(tolua_S);
	lua_register_cocos2dx_ui_Slider(tolua_S);
	lua_register_cocos2dx_ui_ScrollView(tolua_S);
	lua_register_cocos2dx_ui_ListView(tolua_S);
	lua_register_cocos2dx_ui_Button(tolua_S);
	lua_register_cocos2dx_ui_LayoutParameter(tolua_S);
	lua_register_cocos2dx_ui_LinearLayoutParameter(tolua_S);
	lua_register_cocos2dx_ui_ImageView(tolua_S);
	lua_register_cocos2dx_ui_HBox(tolua_S);
	lua_register_cocos2dx_ui_RichElementText(tolua_S);
	lua_register_cocos2dx_ui_PageView(tolua_S);
	lua_register_cocos2dx_ui_Helper(tolua_S);
	lua_register_cocos2dx_ui_EditBox(tolua_S);
	lua_register_cocos2dx_ui_Text(tolua_S);
	lua_register_cocos2dx_ui_RichText(tolua_S);
	lua_register_cocos2dx_ui_RichElementImage(tolua_S);
	lua_register_cocos2dx_ui_RelativeLayoutParameter(tolua_S);

	tolua_endmodule(tolua_S);
	return 1;
}

