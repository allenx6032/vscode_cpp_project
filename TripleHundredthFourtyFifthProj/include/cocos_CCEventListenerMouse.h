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

#include "cocos_CCEventListener.h"
#include "cocos_CCEventMouse.h"

#ifndef __cocos2d_libs__CCMouseEventListener__
#define __cocos2d_libs__CCMouseEventListener__

NS_CC_BEGIN

class Event;

class CC_DLL EventListenerMouse : public EventListener
{
public:
    static const std::string LISTENER_ID;
    
    static EventListenerMouse* create();

    /// Overrides
    virtual EventListenerMouse* clone() override;
    virtual bool checkAvailable() override;

    std::function<void(Event* event)> onMouseDown;
    std::function<void(Event* event)> onMouseUp;
    std::function<void(Event* event)> onMouseMove;
    std::function<void(Event* event)> onMouseScroll;

CC_CONSTRUCTOR_ACCESS:
    EventListenerMouse();
    bool init();
};

NS_CC_END

#endif /* defined(__cocos2d_libs__CCMouseEventListener__) */
