#include "cocos_ccConfig.h"
#if CC_USE_PHYSICS
#ifndef __cocos2dx_physics_h__
#define __cocos2dx_physics_h__

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

int register_all_cocos2dx_physics(lua_State* tolua_S);

#endif // __cocos2dx_physics_h__
#endif //#if CC_USE_PHYSICS
