/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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

#ifndef __PLATFORM_CCGL_H__
#define __PLATFORM_CCGL_H__

#include "cocos_CCPlatformConfig.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#include "cocos_CCGL-mac.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "cocos_CCGL-ios.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "cocos_CCGL-android.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include "cocos_CCGL-win32.h"
#elif CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
#include "cocos_CCGL-linux.h"
#endif

#endif /* __PLATFORM_CCPLATFORMDEFINE_H__*/
