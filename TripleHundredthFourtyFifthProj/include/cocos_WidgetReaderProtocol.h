/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 
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

#ifndef __TestCpp__WidgetReaderProtocol__
#define __TestCpp__WidgetReaderProtocol__

#include "cocos_cocos2d.h"
#include "cocos_DictionaryHelper.h"
#include "cocos_CocosStudioExport.h"

namespace protocolbuffers
{
    class NodeTree;
}

namespace cocos2d
{
    namespace ui
    {
        class Widget;
    }
}

namespace cocostudio
{
    class CocoLoader;
    struct stExpCocoNode;
    
    class CC_STUDIO_DLL WidgetReaderProtocol
    {
    public:
        virtual ~WidgetReaderProtocol() {};
        virtual void setPropsFromJsonDictionary(cocos2d::ui::Widget* widget, const rapidjson::Value& options) = 0;
        virtual void setPropsFromBinary(cocos2d::ui::Widget* widget, CocoLoader* cocoLoader,  stExpCocoNode*	pCocoNode) = 0;        
    };
}

#endif /* defined(__TestCpp__WidgetReaderProtocol__) */
