/****************************************************************************
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

#include "lua_cocos2dx_deprecated.h"

#include "tolua_fix.h"
#include "lua_cocos2dx_LuaBasicConversions.h"
#include "lua_cocos2dx_LuaScriptHandlerMgr.h"
#include "lua_cocos2dx_CCLuaValue.h"

#include "cocos2d-CCDeprecated.h"


USING_NS_CC;

#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 //vs 2005 or higher
#pragma warning (push)
#pragma warning (disable: 4996)
#endif

int register_all_cocos2dx_deprecated(lua_State* tolua_S) {
    return 0;
}
int register_all_cocos2dx_manual_deprecated(lua_State* tolua_S) {
    return 0;
}
