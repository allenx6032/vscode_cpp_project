/***************************************************************************
 Copyright (c) 2012 Zynga Inc.
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2013-2014 Chukong Technologic Inc.

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
***************************************************************************/
 

/*
 Local Storage support for the JS Bindings for iOS.
 Works on cocos2d-iphone and cocos2d-x.
 */

#include "LocalStorage.h"
#include "cocos_CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "jni.h"
#include "jni/JniHelper.h"

USING_NS_CC;
static int _initialized = 0;

static void splitFilename (std::string& str)
{
	size_t found = 0;
	found=str.find_last_of("/\\");
	if (found != std::string::npos)
	{
		str = str.substr(found+1);
	}
}

void localStorageInit( const std::string& fullpath)
{
	if (fullpath.empty())
        return;

	if( ! _initialized )
    {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/lib/Cocos2dxLocalStorage", "init", "(Ljava/lang/String;Ljava/lang/String;)Z")) {
            std::string strDBFilename = fullpath;
            splitFilename(strDBFilename);
            jstring jdbName = t.env->NewStringUTF(strDBFilename.c_str());
            jstring jtableName = t.env->NewStringUTF("data");
            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, jdbName, jtableName);
            t.env->DeleteLocalRef(jdbName);
            t.env->DeleteLocalRef(jtableName);
            t.env->DeleteLocalRef(t.classID);
            if (ret) {
                _initialized = 1;
            }
        }
	}
}

void localStorageFree()
{
	if( _initialized ) {
		
		JniMethodInfo t;
        
        if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/lib/Cocos2dxLocalStorage", "destory", "()V"))
        {
            t.env->CallStaticVoidMethod(t.classID, t.methodID);
        	t.env->DeleteLocalRef(t.classID); 
        }
        
		_initialized = 0;
	}
}

/** sets an item in the LS */
void localStorageSetItem( const std::string& key, const std::string& value)
{
	assert( _initialized );
	
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/lib/Cocos2dxLocalStorage", "setItem", "(Ljava/lang/String;Ljava/lang/String;)V")) {
        jstring jkey = t.env->NewStringUTF(key.c_str());
        jstring jvalue = t.env->NewStringUTF(value.c_str());
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jkey, jvalue);
        t.env->DeleteLocalRef(jkey);
        t.env->DeleteLocalRef(jvalue);
        t.env->DeleteLocalRef(t.classID);
    }
}

/** gets an item from the LS */
std::string localStorageGetItem( const std::string& key )
{
	assert( _initialized );
    JniMethodInfo t;

    std::string ret;
    if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/lib/Cocos2dxLocalStorage", "getItem", "(Ljava/lang/String;)Ljava/lang/String;")) {
        jstring jkey = t.env->NewStringUTF(key.c_str());
        jstring jret = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID, jkey);
        ret = JniHelper::jstring2string(jret);
        t.env->DeleteLocalRef(jret);
        t.env->DeleteLocalRef(jkey);
        t.env->DeleteLocalRef(t.classID);
    }
    return ret;
}

/** removes an item from the LS */
void localStorageRemoveItem( const std::string& key )
{
	assert( _initialized );
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, "org/cocos2dx/lib/Cocos2dxLocalStorage", "removeItem", "(Ljava/lang/String;)V")) {
        jstring jkey = t.env->NewStringUTF(key.c_str());
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jkey);
        t.env->DeleteLocalRef(jkey);
        t.env->DeleteLocalRef(t.classID);
    }

}

#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
