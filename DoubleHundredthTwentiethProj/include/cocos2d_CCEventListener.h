/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#ifndef __CCEVENTLISTENER_H__
#define __CCEVENTLISTENER_H__

#include <functional>
#include <string>
#include <memory>

#include "cocos2d_CCPlatformMacros.h"
#include "cocos2d_CCRef.h"

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

class Event;

/** @class EventListener
 *  @brief The base class of event listener.
 *  If you need custom listener which with different callback, you need to inherit this class.
 *  For instance, you could refer to EventListenerKeyboard, EventListenerCustom.
 */
class CC_DLL EventListener : public Ref
{
public:
    /**
     * Constructor
     * @js ctor
     */
    EventListener();

    /** 
     * Initializes event with type and callback function
     * @js NA
     */
    bool init(const std::string& listenerID, const std::function<void(Event*)>& callback);

public:
    /** Destructor.
     * @js NA 
     */
    virtual ~EventListener();

protected:
};

NS_CC_END

// end of base group
/// @}

#endif // __CCEVENTLISTENER_H__
