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

#include "cocos_UIRelativeBox.h"

NS_CC_BEGIN

namespace ui{
    
RelativeBox::RelativeBox()
{
}

RelativeBox::~RelativeBox()
{
}

RelativeBox* RelativeBox::create()
{
    RelativeBox* widget = new (std::nothrow) RelativeBox();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

RelativeBox* RelativeBox::create(const cocos2d::Size &size)
{
    RelativeBox* widget = new (std::nothrow) RelativeBox();
    if (widget && widget->initWithSize(size))
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

bool RelativeBox::init()
{
    if (Layout::init())
    {
        setLayoutType(Layout::Type::RELATIVE);
        return true;
    }
    return false;
}

bool RelativeBox::initWithSize(const Size& size)
{
    if (init())
    {
        setContentSize(size);
        return true;
    }
    return false;
}
    
}

NS_CC_END
