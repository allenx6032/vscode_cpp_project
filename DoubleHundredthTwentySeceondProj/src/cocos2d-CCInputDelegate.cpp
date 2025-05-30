/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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


#include "cocos2d-CCConfig.h"
#if CC_USE_CCS > 0


#include "cocos2d-CCInputDelegate.h"
#include "cocos2d-CCDirector.h"
#include "cocos2d-CCDevice.h"
#include "cocos2d-CCEventListenerTouch.h"
#include "cocos2d-CCEventListenerAcceleration.h"
#include "cocos2d-CCEventListenerKeyboard.h"
#include "cocos2d-CCEventDispatcher.h"

using namespace cocos2d;

namespace cocostudio {

InputDelegate::InputDelegate(void)
: _touchEnabled(false)
, _touchListener(nullptr)
, _accelerometerEnabled(false)
, _accelerometerListener(nullptr)
, _keypadEnabled(false)
, _keyboardListener(nullptr)
, _touchPriority(-1)
, _touchMode(Touch::DispatchMode::ALL_AT_ONCE)
{

}

InputDelegate::~InputDelegate(void)
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->removeEventListener(_touchListener);
    dispatcher->removeEventListener(_keyboardListener);
    dispatcher->removeEventListener(_accelerometerListener);
    Device::setAccelerometerEnabled(false);
}
    
void InputDelegate::didAccelerate(cocos2d::Acceleration* /*accelerationValue*/)
{}

bool InputDelegate::ccTouchBegan(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
{
    return false;
}

void InputDelegate::ccTouchMoved(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::ccTouchEnded(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::ccTouchCancelled(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::ccTouchesBegan(cocos2d::__Set* /*touches*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::ccTouchesMoved(cocos2d::__Set* /*touches*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::ccTouchesEnded(cocos2d::__Set* /*touches*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::ccTouchesCancelled(cocos2d::__Set* /*touches*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::onAcceleration(cocos2d::Acceleration* /*acc*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::onKeyPressed(cocos2d::EventKeyboard::KeyCode /*keyCode*/, cocos2d::Event* /*event*/)
{}

void InputDelegate::onKeyReleased(cocos2d::EventKeyboard::KeyCode /*keyCode*/, cocos2d::Event* /*event*/)
{}
    
bool InputDelegate::onTouchBegan(Touch* /*pTouch*/, Event* /*pEvent*/)
{
    return true;
}

void InputDelegate::onTouchMoved(Touch* /*pTouch*/, Event* /*pEvent*/)
{
}
    
void InputDelegate::onTouchEnded(Touch* /*pTouch*/, Event* /*pEvent*/)
{
}

void InputDelegate::onTouchCancelled(Touch* /*pTouch*/, Event* /*pEvent*/)
{
}    

void InputDelegate::onTouchesBegan(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/)
{
}

void InputDelegate::onTouchesMoved(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/)
{
}

void InputDelegate::onTouchesEnded(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/)
{
}

void InputDelegate::onTouchesCancelled(const std::vector<Touch*>& /*pTouches*/, Event* /*pEvent*/)
{
}

bool InputDelegate::isTouchEnabled() const
{
    return _touchEnabled;
}

void InputDelegate::setTouchEnabled(bool enabled)
{
    if (_touchEnabled != enabled)
    {
        auto dispatcher = Director::getInstance()->getEventDispatcher();
        _touchEnabled = enabled;
        if (enabled)
        {
            if( _touchMode == Touch::DispatchMode::ALL_AT_ONCE ) {
                // Register Touch Event
                auto listener = EventListenerTouchAllAtOnce::create();

                listener->onTouchesBegan = CC_CALLBACK_2(InputDelegate::onTouchesBegan, this);
                listener->onTouchesMoved = CC_CALLBACK_2(InputDelegate::onTouchesMoved, this);
                listener->onTouchesEnded = CC_CALLBACK_2(InputDelegate::onTouchesEnded, this);
                listener->onTouchesCancelled = CC_CALLBACK_2(InputDelegate::onTouchesCancelled, this);

                dispatcher->addEventListenerWithFixedPriority(listener, _touchPriority);
                _touchListener = listener;
            } else {
                // Register Touch Event
                auto listener = EventListenerTouchOneByOne::create();
                listener->setSwallowTouches(true);

                listener->onTouchBegan = CC_CALLBACK_2(InputDelegate::onTouchBegan, this);
                listener->onTouchMoved = CC_CALLBACK_2(InputDelegate::onTouchMoved, this);
                listener->onTouchEnded = CC_CALLBACK_2(InputDelegate::onTouchEnded, this);
                listener->onTouchCancelled = CC_CALLBACK_2(InputDelegate::onTouchCancelled, this);

                dispatcher->addEventListenerWithFixedPriority(listener, _touchPriority);
                _touchListener = listener;
            }
        }
        else
        {
            dispatcher->removeEventListener(_touchListener);
        }
    }
}

void InputDelegate::setTouchMode(Touch::DispatchMode mode)
{
    if(_touchMode != mode)
    {
        _touchMode = mode;

		if( _touchEnabled)
        {
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
    }
}

void InputDelegate::setTouchPriority(int priority)
{
    if (_touchPriority != priority)
    {
        _touchPriority = priority;

		if( _touchEnabled)
        {
			setTouchEnabled(false);
			setTouchEnabled(true);
		}
    }
}

int InputDelegate::getTouchPriority() const
{
    return _touchPriority;
}

Touch::DispatchMode InputDelegate::getTouchMode() const
{
    return _touchMode;
}

bool InputDelegate::isAccelerometerEnabled() const
{
    return _accelerometerEnabled;
}

void InputDelegate::setAccelerometerEnabled(bool enabled)
{
    if (enabled != _accelerometerEnabled)
    {
        _accelerometerEnabled = enabled;

        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeEventListener(_accelerometerListener);
        _accelerometerListener = nullptr;

        Device::setAccelerometerEnabled(enabled);

        if (enabled)
        {
            auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(InputDelegate::onAcceleration, this));
            dispatcher->addEventListenerWithFixedPriority(listener, -1);
            _accelerometerListener = listener;
        }
    }
}

bool InputDelegate::isKeypadEnabled() const
{
    return _keypadEnabled;
}

void InputDelegate::setKeypadEnabled(bool enabled)
{
    if (enabled != _keypadEnabled)
    {
        _keypadEnabled = enabled;

        auto dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeEventListener(_keyboardListener);

        if (enabled)
        {
            auto listener = EventListenerKeyboard::create();
            listener->onKeyPressed = CC_CALLBACK_2(InputDelegate::onKeyPressed, this);
            listener->onKeyReleased = CC_CALLBACK_2(InputDelegate::onKeyReleased, this);

            dispatcher->addEventListenerWithFixedPriority(listener, -1);
            _keyboardListener = listener;
        }
    }
}


}


#endif // CC_USE_CCS

