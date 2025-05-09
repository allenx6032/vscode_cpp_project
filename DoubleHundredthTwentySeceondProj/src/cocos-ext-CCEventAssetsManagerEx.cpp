/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#include "cocos-ext-CCEventAssetsManagerEx.h"
#include "cocos2d-CCMacros.h"
#include <functional>
#include "cocos-ext-AssetsManagerEx.h"

NS_CC_EXT_BEGIN

EventAssetsManagerEx::EventAssetsManagerEx(const std::string& eventName, cocos2d::extension::AssetsManagerEx *manager, const EventCode &code, float percent/* = 0 */, float percentByFile/* = 0*/, const std::string& assetId/* = "" */, const std::string& message/* = "" */, int curle_code/* = CURLE_OK*/, int curlm_code/* = CURLM_OK*/)
: EventCustom(eventName)
, _code(code)
, _manager(manager)
, _message(message)
, _assetId(assetId)
, _curle_code(curle_code)
, _curlm_code(curlm_code)
, _percent(percent)
, _percentByFile(percentByFile)
{
}


NS_CC_EXT_END
