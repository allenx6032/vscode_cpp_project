#include "cocos_ccConfig.h"
#ifndef __cocos2dx_experimental_h__
#define __cocos2dx_experimental_h__

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

int register_all_cocos2dx_experimental(lua_State* tolua_S);

#endif // __cocos2dx_experimental_h__
