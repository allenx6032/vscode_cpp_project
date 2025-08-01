/****************************************************************************
Copyright (c) 2009      Sindesso Pty Ltd http://www.sindesso.com/
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

#include "cocos_CCTransitionPageTurn.h"
#include "cocos_CCDirector.h"
#include "cocos_CCActionPageTurn3D.h"
#include "cocos_CCNodeGrid.h"
#include "cocos_CCRenderer.h"

NS_CC_BEGIN

TransitionPageTurn::TransitionPageTurn()
{
    _inSceneProxy = NodeGrid::create();
    _outSceneProxy = NodeGrid::create();
    
    _inSceneProxy->retain();
    _outSceneProxy->retain();
}

TransitionPageTurn::~TransitionPageTurn()
{
    CC_SAFE_RELEASE(_inSceneProxy);
    CC_SAFE_RELEASE(_outSceneProxy);
}

/** creates a base transition with duration and incoming scene */
TransitionPageTurn * TransitionPageTurn::create(float t, Scene *scene, bool backwards)
{
    TransitionPageTurn * transition = new (std::nothrow) TransitionPageTurn();
    transition->initWithDuration(t,scene,backwards);
    transition->autorelease();
    return transition;
}

/** initializes a transition with duration and incoming scene */
bool TransitionPageTurn::initWithDuration(float t, Scene *scene, bool backwards)
{
    // FIXME:: needed before [super init]
    _back = backwards;

    if (TransitionScene::initWithDuration(t, scene))
    {
        // do something
    }
    return true;
}

void TransitionPageTurn::sceneOrder()
{
    _isInSceneOnTop = _back;
}

void TransitionPageTurn::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    Scene::draw(renderer, transform, flags);
    
    if( _isInSceneOnTop ) {
        _outSceneProxy->visit(renderer, transform, flags);
        _inSceneProxy->visit(renderer, transform, flags);
    } else {
        _inSceneProxy->visit(renderer, transform, flags);
        _outSceneProxy->visit(renderer, transform, flags);
        
    }
}

void TransitionPageTurn::onEnter()
{
    TransitionScene::onEnter();

    _inSceneProxy->setTarget(_inScene);
    _outSceneProxy->setTarget(_outScene);

    _inSceneProxy->onEnter();
    _outSceneProxy->onEnter();
    
    Size s = Director::getInstance()->getWinSize();
    int x,y;
    if (s.width > s.height)
    {
        x=16;
        y=12;
    }
    else
    {
        x=12;
        y=16;
    }

    ActionInterval *action  = this->actionWithSize(Size(x,y));

    if (! _back )
    {
        _outSceneProxy->runAction
        (
            Sequence::create
            (
                action,
                CallFunc::create(CC_CALLBACK_0(TransitionScene::finish,this)),
                StopGrid::create(),
                nullptr
            )
        );
    }
    else
    {
        // to prevent initial flicker
        _inSceneProxy->setVisible(false);
        _inSceneProxy->runAction
        (
            Sequence::create
            (
                Show::create(),
                action,
                CallFunc::create(CC_CALLBACK_0(TransitionScene::finish,this)),
                StopGrid::create(),
                nullptr
            )
        );
    }
}
void TransitionPageTurn::onExit()
{
    _outSceneProxy->setTarget(nullptr);
    _outSceneProxy->setTarget(nullptr);
    _outSceneProxy->onExit();
    _inSceneProxy->onExit();
    
    TransitionScene::onExit();
}

ActionInterval* TransitionPageTurn:: actionWithSize(const Size& vector)
{
    if (_back)
    {
        // Get hold of the PageTurn3DAction
        return ReverseTime::create
        (
            PageTurn3D::create(_duration, vector)
        );
    }
    else
    {
        // Get hold of the PageTurn3DAction
        return PageTurn3D::create(_duration, vector);
    }
}

NS_CC_END
