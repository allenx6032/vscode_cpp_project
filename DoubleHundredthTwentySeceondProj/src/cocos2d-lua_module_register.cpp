/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "cocos2d-CCConfig.h"

#include "cocos2d-lua_module_register.h"

#include "cocos2d-lua_cocos2dx_cocosdenshion_manual.h"
#include "cocos2d-lua_cocos2dx_network_manual.h"
// #include "cocos2d-cocosbuilder/lua_cocos2dx_cocosbuilder_manual.h"

#if CC_USE_CCS > 0
    #include "cocos2d-lua_cocos2dx_coco_studio_manual.hpp"
#endif

 #include "cocos2d-lua_cocos2dx_extension_manual.h"

#if CC_USE_UI > 0
#include "cocos2d-lua_cocos2dx_ui_manual.hpp"
#endif

#include "cocos2d-lua_cocos2dx_spine_manual.hpp"
// #include "cocos2d-3d/lua_cocos2dx_3d_manual.h"
#include "cocos2d-lua_cocos2dx_audioengine_manual.h"
// #include "cocos2d-physics3d/lua_cocos2dx_physics3d_manual.h"
// #include "cocos2d-navmesh/lua_cocos2dx_navmesh_manual.h"


int lua_module_register(lua_State* L)
{
    //Dont' change the module register order unless you know what your are doing
#if CC_USE_SIMPLE_AUDIO > 0
     register_cocosdenshion_module(L);
#endif

#if CC_USE_NETWORK > 0
    register_network_module(L);
#endif

    // register_cocosbuilder_module(L);

#if CC_USE_CCS > 0
    register_cocostudio_module(L);
#endif

#if CC_USE_UI > 0
	register_ui_module(L);
#endif

     register_extension_module(L);
#if CC_USE_SPINE > 0
    register_spine_module(L);
#endif
    // register_cocos3d_module(L);

#if CC_USE_AUDIO_ENGINE > 0
    register_audioengine_module(L);
#endif

#if CC_USE_3D_PHYSICS && CC_ENABLE_BULLET_INTEGRATION
    register_physics3d_module(L);
#endif
#if CC_USE_NAVMESH
    register_navmesh_module(L);
#endif
    return 1;
}

