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

#ifndef COCOS2DX_SCRIPT_LUA_COCOS2DX_SUPPORT_PLATFORM_ANDROID_JNI_JAVA_ORG_COCOS2DX_LIB_COCOS2DX_LUA_JAVA_BRIDEG_H
#define COCOS2DX_SCRIPT_LUA_COCOS2DX_SUPPORT_PLATFORM_ANDROID_JNI_JAVA_ORG_COCOS2DX_LIB_COCOS2DX_LUA_JAVA_BRIDEG_H

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>

extern "C" {

/// @cond
/*
 * Class:     Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge
 * Method:    callLuaFunctionWithString
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge_callLuaFunctionWithString
  (JNIEnv *, jclass, jint, jstring);

/*
 * Class:     Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge
 * Method:    callLuaGlobalFunctionWithString
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge_callLuaGlobalFunctionWithString
  (JNIEnv *env, jclass, jstring, jstring);

/*
 * Class:     Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge
 * Method:    retainLuaFunction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge_retainLuaFunction
  (JNIEnv *env, jclass, jint);

/*
 * Class:     Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge
 * Method:    releaseLuaFunction
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_org_cocos2dx_lib_Cocos2dxLuaJavaBridge_releaseLuaFunction
  (JNIEnv *env, jclass, jint);

} // extern "C"
/// @endcond
#endif // COCOS2DX_SCRIPT_LUA_COCOS2DX_SUPPORT_PLATFORM_ANDROID_JNI_JAVA_ORG_COCOS2DX_LIB_COCOS2DX_LUA_JAVA_BRIDEG_H
#endif
