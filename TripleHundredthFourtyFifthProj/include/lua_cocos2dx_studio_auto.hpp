#include "cocos_ccConfig.h"
#ifndef __cocos2dx_studio_h__
#define __cocos2dx_studio_h__

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

int register_all_cocos2dx_studio(lua_State* tolua_S);

#endif // __cocos2dx_studio_h__
