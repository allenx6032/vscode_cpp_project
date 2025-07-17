#ifndef __LUA_TEMPLATE_RUNTIME_FRAMEWORKS_RUNTIME_SRC_CLASSES_LUA_MODULE_REGISTER_H__
#define __LUA_TEMPLATE_RUNTIME_FRAMEWORKS_RUNTIME_SRC_CLASSES_LUA_MODULE_REGISTER_H__

#include "lua_cocos2dx_lua_audio_manual.h"
#include "lua_cocos2dx_network_manual.h"
#include "lua_cocos2dx_coco_studio_manual.hpp"
#include "lua_cocos2dx_ui_manual.hpp"
#include "lua_cocos2dx_spine_manual.hpp"
#include "lua_cocos2dx_3d_manual.h"
#include "lua_cocos2dx_quick_manual.hpp"
#include "lua_cocos2dx_lua_dragonBones.hpp"

int lua_module_register(lua_State* L)
{
    //Dont' change the module register order unless you know what your are doing
    register_audio_module(L);
    register_network_module(L);
    register_cocostudio_module(L);
    register_ui_moudle(L);
    register_spine_module(L);
    register_cocos3d_module(L);
    register_dragonBones_manual(L);
    return 1;
}

#endif  // __LUA_TEMPLATE_RUNTIME_FRAMEWORKS_RUNTIME_SRC_CLASSES_LUA_MODULE_REGISTER_H__

