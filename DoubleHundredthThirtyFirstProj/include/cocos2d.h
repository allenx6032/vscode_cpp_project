/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

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

#ifndef __COCOS2D_H__
#define __COCOS2D_H__

// 0x00 HI ME LO
// 00   03 08 00
#define COCOS2D_VERSION 0x00031101

//
// all cocos2d include files
//
#include "cocos2d-ccConfig.h"

// base
#include "cocos2d-ccAsyncTaskPool.h"
#include "cocos2d-ccAutoreleasePool.h"
#include "cocos2d-ccConfiguration.h"
#include "cocos2d-ccConsole.h"
#include "cocos2d-ccData.h"
#include "cocos2d-ccDirector.h"
#include "cocos2d-ccIMEDelegate.h"
#include "cocos2d-ccIMEDispatcher.h"
#include "cocos2d-ccMap.h"
#include "cocos2d-ccNS.h"
#include "cocos2d-ccProfiling.h"
#include "cocos2d-ccRef.h"
#include "cocos2d-ccScheduler.h"
#include "cocos2d-ccUserDefault.h"
#include "cocos2d-ccValue.h"
#include "cocos2d-ccVector.h"
#include "cocos2d-ZipUtils.h"
#include "cocos2d-base64.h"
#include "cocos2d-ccConfig.h"
#include "cocos2d-ccMacros.h"
#include "cocos2d-ccTypes.h"
#include "cocos2d-ccUTF8.h"
#include "cocos2d-ccUtils.h"

// EventDispatcher
#include "cocos2d-ccEventAcceleration.h"
#include "cocos2d-ccEventCustom.h"
#include "cocos2d-ccEventDispatcher.h"
//#include "cocos2d-ccEventFocus.h"
#include "cocos2d-ccEventKeyboard.h"
#include "cocos2d-ccEventListenerAcceleration.h"
#include "cocos2d-ccEventListenerCustom.h"
//#include "cocos2d-ccEventListenerFocus.h"
#include "cocos2d-ccEventListenerKeyboard.h"
#include "cocos2d-ccEventListenerMouse.h"
#include "cocos2d-ccEventListenerTouch.h"
#include "cocos2d-ccEventMouse.h"
#include "cocos2d-ccEventTouch.h"
#include "cocos2d-ccEventType.h"

// math
#include "cocos2d-CCAffineTransform.h"
#include "cocos2d-CCGeometry.h"
#include "cocos2d-CCVertex.h"
#include "cocos2d-Mat4.h"
#include "cocos2d-MathUtil.h"
#include "cocos2d-Quaternion.h"
#include "cocos2d-Vec2.h"
#include "cocos2d-Vec3.h"
#include "cocos2d-Vec4.h"

// actions
#include "cocos2d-CCAction.h"
#include "cocos2d-CCActionCamera.h"
#include "cocos2d-CCActionCatmullRom.h"
#include "cocos2d-CCActionEase.h"
#include "cocos2d-CCActionGrid.h"
#include "cocos2d-CCActionGrid3D.h"
#include "cocos2d-CCActionInstant.h"
#include "cocos2d-CCActionInterval.h"
#include "cocos2d-CCActionManager.h"
#include "cocos2d-CCActionPageTurn3D.h"
#include "cocos2d-CCActionProgressTimer.h"
#include "cocos2d-CCActionTiledGrid.h"
#include "cocos2d-CCActionTween.h"
#include "cocos2d-CCTweenFunction.h"

// 2d nodes
#include "cocos2d-CCAtlasNode.h"
#include "cocos2d-CCClippingNode.h"
#include "cocos2d-CCClippingRectangleNode.h"
#include "cocos2d-CCDrawNode.h"
#include "cocos2d-CCDrawingPrimitives.h"
#include "cocos2d-CCFontFNT.h"
#include "cocos2d-CCLabel.h"
#include "cocos2d-CCLabelAtlas.h"
//#include "cocos2d-CCLabelBMFont.h"
//#include "cocos2d-CCLabelTTF.h"
#include "cocos2d-CCLayer.h"
#include "cocos2d-CCMotionStreak.h"
#include "cocos2d-CCNode.h"
#include "cocos2d-CCNodeGrid.h"
#include "cocos2d-CCParticleBatchNode.h"
#include "cocos2d-CCParticleExamples.h"
#include "cocos2d-CCParticleSystem.h"
#include "cocos2d-CCParticleSystemQuad.h"
#include "cocos2d-CCProgressTimer.h"
#include "cocos2d-CCRenderTexture.h"
#include "cocos2d-CCScene.h"
#include "cocos2d-CCTransition.h"
#include "cocos2d-CCTransitionPageTurn.h"
#include "cocos2d-CCTransitionProgress.h"

// 2d utils
#include "cocos2d-CCCamera.h"
#include "cocos2d-CCGrabber.h"
#include "cocos2d-CCGrid.h"

// include
#include "cocos2d-ccProtocols.h"

// renderer
#include "cocos2d-CCCustomCommand.h"
#include "cocos2d-CCGLProgram.h"
#include "cocos2d-CCGLProgramCache.h"
#include "cocos2d-CCGLProgramState.h"
#include "cocos2d-CCGroupCommand.h"
#include "cocos2d-CCPrimitive.h"
#include "cocos2d-CCPrimitiveCommand.h"
#include "cocos2d-CCQuadCommand.h"
#include "cocos2d-CCRenderCommand.h"
#include "cocos2d-CCRenderCommandPool.h"
#include "cocos2d-CCRenderState.h"
#include "cocos2d-CCRenderer.h"
#include "cocos2d-CCTexture2D.h"
#include "cocos2d-CCTextureCache.h"
#include "cocos2d-CCTrianglesCommand.h"
//#include "cocos2d-CCVertexAttribBinding.h"
#include "cocos2d-CCVertexIndexBuffer.h"
#include "cocos2d-CCVertexIndexData.h"
#include "cocos2d-CCPrimitive.h"
#include "cocos2d-CCPrimitiveCommand.h"
#include "cocos2d-CCTrianglesCommand.h"
#include "cocos2d-CCRenderState.h"
#include "cocos2d-CCFrameBuffer.h"
#include "cocos2d-CCGLStateCache.h"
#include "cocos2d-CCShaders.h"

// platform
#include "cocos2d-CCCommon.h"
#include "cocos2d-CCDevice.h"
#include "cocos2d-CCFileUtils.h"
#include "cocos2d-CCImage.h"
#include "cocos2d-CCPlatformConfig.h"
#include "cocos2d-CCPlatformMacros.h"
#include "cocos2d-CCSAXParser.h"
#include "cocos2d-CCThread.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    #include "cocos2d-CCApplication-ios.h"
    #include "cocos2d-CCGLViewImpl-ios.h"
    #include "cocos2d-CCGL-ios.h"
    #include "cocos2d-CCStdC-ios.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "cocos2d-CCApplication-android.h"
    #include "cocos2d-CCGLViewImpl-android.h"
    #include "cocos2d-CCGL-android.h"
    #include "cocos2d-CCStdC-android.h"
//Enhance modification begin
    #include "cocos2d-CCEnhanceAPI-android.h"
//Enhance modification end
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#if (CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY)
    #include "platform/blackberry/CCApplication.h"
    #include "platform/blackberry/CCGLViewImpl.h"
    #include "platform/blackberry/CCGL.h"
    #include "platform/blackberry/CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_BLACKBERRY

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    #include "cocos2d-CCApplication-win32.h"
    #include "cocos2d-CCGLViewImpl-desktop.h"
    #include "cocos2d-CCGL-win32.h"
    #include "cocos2d-CCStdC-win32.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    #include "cocos2d-CCGLViewImpl-desktop.h"
    #include "cocos2d-CCApplication-mac.h"
    #include "cocos2d-CCGL-mac.h"
    #include "cocos2d-CCStdC-mac.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#if (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    #include "cocos2d-CCApplication-linux.h"
    #include "cocos2d-CCGLViewImpl-desktop.h"
    #include "cocos2d-CCGL-linux.h"
    #include "cocos2d-CCStdC-linux.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	#include "cocos2d-CCApplication.h"
	#include "cocos2d-CCGLViewImpl-winrt.h"
	#include "cocos2d-CCGL.h"
	#include "cocos2d-CCStdC.h"
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WINRT

// script_support
#include "cocos2d-ccScriptSupport.h"

// sprite_nodes
#include "cocos2d-CCAnimation.h"
#include "cocos2d-CCAnimationCache.h"
#include "cocos2d-CCSprite.h"
#if CC_USE_AUTO_POLYGON > 0
#include "cocos2d-CCAutoPolygon.h"
#endif // CC_USE_AUTO_POLYGON
#include "cocos2d-CCSpriteBatchNode.h"
#include "cocos2d-CCSpriteFrame.h"
#include "cocos2d-CCSpriteFrameCache.h"

// text_input_node
#include "cocos2d-CCTextFieldTTF.h"

// textures
#include "cocos2d-CCTextureAtlas.h"

#if CC_USE_3D
//3d
#include "3d/CCAABB.h"
#include "3d/CCAnimate3D.h"
#include "3d/CCAnimation3D.h"
#include "3d/CCAttachNode.h"
#include "3d/CCBillBoard.h"
#include "3d/CCFrustum.h"
#include "3d/CCMesh.h"
#include "3d/CCMeshSkin.h"
#include "3d/CCMotionStreak3D.h"
#include "3d/CCMeshVertexIndexData.h"
#include "3d/CCOBB.h"
#include "3d/CCPlane.h"
#include "3d/CCRay.h"
#include "3d/CCSkeleton3D.h"
#include "3d/CCSkybox.h"
#include "3d/CCSprite3D.h"
#include "3d/CCSprite3DMaterial.h"
#include "3d/CCTerrain.h"
#endif // CC_USE_3D

// Deprecated
// All deprecated features are include inside deprecated/CCDeprecated.h.
// It is recommanded that you just inlcude what is needed.
// eg. #include "cocos2d-CCString.h" if you only need cocos2d::__String.
#include "cocos2d-CCDeprecated.h"


NS_CC_BEGIN

CC_DLL const char* cocos2dVersion();

NS_CC_END

#endif // __COCOS2D_H__
