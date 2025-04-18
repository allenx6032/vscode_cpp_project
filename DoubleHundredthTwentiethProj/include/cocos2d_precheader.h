// precompiled header
// include here heavy headers which are included in many files
// this will speed-up build a lot
#pragma once

// types
#include <limits>
#include <string>
#include <cctype>
#include <sstream>

// containers
#include <iterator>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>

// utilities
#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include <functional>
#include <algorithm>
#include <utility>
#include <tuple>

// cocos
/*
#include "cocos2d_ccConfig.h"

// base
#include "cocos2d_CCAsyncTaskPool.h"
#include "cocos2d_CCAutoreleasePool.h"
#include "cocos2d_CCConfiguration.h"
#include "cocos2d_CCData.h"
#include "cocos2d_CCDirector.h"
#include "cocos2d_CCIMEDelegate.h"
#include "cocos2d_CCIMEDispatcher.h"
#include "cocos2d_CCProperties.h"
#include "cocos2d_CCRef.h"
#include "cocos2d_CCRefPtr.h"
#include "cocos2d_CCScheduler.h"
#include "cocos2d_CCValue.h"
#include "cocos2d_CCVector.h"
#include "cocos2d_base64.h"
#include "cocos2d_ccConfig.h"
#include "cocos2d_ccMacros.h"
#include "cocos2d_ccTypes.h"
#include "cocos2d_ccUTF8.h"
#include "cocos2d_ccUtils.h"

// EventDispatcher
#include "cocos2d_CCEventCustom.h"
#include "cocos2d_CCEventDispatcher.h"
#include "cocos2d_CCEventListenerCustom.h"
#include "cocos2d_CCInputEvents.h"
#include "cocos2d_CCEventController.h"
#include "cocos2d_CCController.h"
#include "cocos2d_CCEventType.h"

// math
#include "cocos2d_CCAffineTransform.h"
#include "cocos2d_CCGeometry.h"
#include "cocos2d_CCVertex.h"
#include "cocos2d_Mat4.h"
#include "cocos2d_MathUtil.h"
#include "cocos2d_Quaternion.h"
#include "cocos2d_Vec2.h"
#include "cocos2d_Vec3.h"
#include "cocos2d_Vec4.h"

// actions
#include "cocos2d_CCAction.h"
#include "cocos2d_CCActionEase.h"
#include "cocos2d_CCActionInstant.h"
#include "cocos2d_CCActionInterval.h"
#include "cocos2d_CCActionManager.h"
#include "cocos2d_CCTweenFunction.h"

// 2d nodes
#include "cocos2d_CCAtlasNode.h"
#include "cocos2d_CCClippingNode.h"
#include "cocos2d_CCDrawNode.h"
#include "cocos2d_CCLabel.h"
#include "cocos2d_CCLayer.h"
#include "cocos2d_CCNode.h"
#include "cocos2d_CCParticleBatchNode.h"
#include "cocos2d_CCParticleSystem.h"
#include "cocos2d_CCParticleSystemQuad.h"
#include "cocos2d_CCRenderTexture.h"
#include "cocos2d_CCScene.h"

// 2d utils
#include "cocos2d_CCCamera.h"

// include
#include "cocos2d_CCProtocols.h"

// renderer
#include "cocos2d_CCCustomCommand.h"
#include "cocos2d_CCGLProgram.h"
#include "cocos2d_CCGLProgramCache.h"
#include "cocos2d_CCGLProgramState.h"
#include "cocos2d_CCGLProgramStateCache.h"
#include "cocos2d_CCGroupCommand.h"
#include "cocos2d_CCMaterial.h"
#include "cocos2d_CCPass.h"
#include "cocos2d_CCQuadCommand.h"
#include "cocos2d_CCRenderCommand.h"
#include "cocos2d_CCRenderCommandPool.h"
#include "cocos2d_CCRenderState.h"
#include "cocos2d_CCRenderer.h"
#include "cocos2d_CCTechnique.h"
#include "cocos2d_CCTexture2D.h"
#include "cocos2d_CCTextureCache.h"
#include "cocos2d_CCTrianglesCommand.h"
#include "cocos2d_CCVertexIndexBuffer.h"
#include "cocos2d_CCVertexIndexData.h"
#include "cocos2d_CCFrameBuffer.h"
#include "cocos2d_ccGLStateCache.h"
#include "cocos2d_ccShaders.h"

// platform
#include "cocos2d_CCCommon.h"
#include "cocos2d_CCDevice.h"
#include "cocos2d_CCFileUtils.h"
#include "cocos2d_CCImage.h"
#include "cocos2d_CCPlatformConfig.h"
#include "cocos2d_CCPlatformMacros.h"
#include "cocos2d_CCSAXParser.h"
#include "cocos2d_CCThread.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    #include "cocos2d_CCApplication-win32.h"
    #include "cocos2d_CCGLViewImpl-desktop.h"
    #include "cocos2d_CCGL-win32.h"
    #include "cocos2d_CCStdC-win32.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    #include "cocos2d_CCGLViewImpl-desktop.h"
    #include "cocos2d_CCApplication-mac.h"
    #include "cocos2d_CCGL-mac.h"
    #include "cocos2d_CCStdC-mac.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    #include "cocos2d_CCApplication-linux.h"
    #include "cocos2d_CCGLViewImpl-desktop.h"
    #include "cocos2d_CCGL-linux.h"
    #include "cocos2d_CCStdC-linux.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

// sprite_nodes
#include "cocos2d_CCSprite.h"
#include "cocos2d_CCSpriteBatchNode.h"

// text_input_node
#include "cocos2d_CCTextFieldTTF.h"

// textures
#include "cocos2d_CCTextureAtlas.h"

// tilemap_parallax_nodes
#include "cocos2d_CCTMXObjectGroup.h"
#include "cocos2d_CCTMXXMLParser.h"
#include "cocos2d_CCFastTMXLayer.h"
#include "cocos2d_CCFastTMXTiledMap.h"
*/
