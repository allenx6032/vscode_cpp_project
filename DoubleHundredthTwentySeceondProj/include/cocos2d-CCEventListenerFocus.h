/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
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

#ifndef __cocos2d_libs__CCEventListenerFocus__
#define __cocos2d_libs__CCEventListenerFocus__

#include "cocos2d-CCEventListener.h"

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

namespace ui {
    class Widget;
}

/** @class EventListenerFocus
 * @brief Focus event listener.
 */
class CC_DLL EventListenerFocus : public EventListener
{
public:
    static const std::string LISTENER_ID;
    
    /** Create a focus event listener.
     *
     * @return An autoreleased EventListenerFocus object.
     */
    static EventListenerFocus* create();
    
    /** Destructor.
     * @js NA
     */
    virtual ~EventListenerFocus();
    
    /// Overrides
    virtual EventListenerFocus* clone() override;
    virtual bool checkAvailable() override;
    //
    
public:
    std::function<void(ui::Widget*, ui::Widget*)> onFocusChanged;
    
CC_CONSTRUCTOR_ACCESS:
    EventListenerFocus();
    bool init();
    
    friend class EventDispatcher;
};

NS_CC_END

// end of base group
/// @}

#endif /* defined(__cocos2d_libs__CCEventListenerFocus__) */
