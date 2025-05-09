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


#include "cocos2d-CCComController.h"
#include "cocos2d-CCNode.h"

namespace cocostudio {

IMPLEMENT_CLASS_COMPONENT_INFO(ComController)

const std::string ComController::COMPONENT_NAME = "CCComController";

ComController::ComController()
{
    _name = COMPONENT_NAME;
}

ComController::~ComController()
{
}

bool ComController::init()
{
    return true;
}

void ComController::onEnter()
{
    if (_owner != nullptr)
    {
        _owner->scheduleUpdate();
    }
}

void ComController::onExit()
{
}

void ComController::onAdd()
{
    if (_owner != nullptr)
    {
       _owner->scheduleUpdate();
    }
}

void ComController::onRemove()
{
}

void ComController::update(float /*delta*/)
{
}

ComController* ComController::create()
{
    ComController * pRet = new (std::nothrow) ComController();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
	return pRet;
}

}


#endif // CC_USE_CCS

