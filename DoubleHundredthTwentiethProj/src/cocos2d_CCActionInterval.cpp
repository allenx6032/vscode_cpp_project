/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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

#include "cocos2d_CCActionInterval.h"

#include <stdarg.h>

#include "cocos2d_CCActionInstant.h"
#include "cocos2d_CCNode.h"
#include "cocos2d_CCSprite.h"
#include "cocos2d_CCConsole.h"
#include "cocos2d_CCDirector.h"
#include "cocos2d_CCEventCustom.h"
#include "cocos2d_CCEventDispatcher.h"
#include "cocos2d_CCStdC.h"

NS_CC_BEGIN

// Extra action for making a Sequence or Spawn when only adding one action to it.
class ExtraAction : public FiniteTimeAction
{
public:
    static ExtraAction* create();
    virtual ExtraAction* clone() const;
    virtual ExtraAction* reverse() const;
    virtual void update(float time);
    virtual void step(float dt);
};

ExtraAction* ExtraAction::create()
{
    ExtraAction* ret = new (std::nothrow) ExtraAction();
    if (ret)
    {
        ret->autorelease();
    }
    return ret;
}

ExtraAction* ExtraAction::clone() const
{
    // no copy constructor
    return ExtraAction::create();
}

ExtraAction* ExtraAction::reverse() const
{
    return ExtraAction::create();
}

void ExtraAction::update(float /*time*/)
{
}

void ExtraAction::step(float /*dt*/)
{
}

//
// IntervalAction
//

bool ActionInterval::initWithDuration(float d)
{
    _duration = d;

    _elapsed = 0;
    _firstTick = true;
    _done = false;
    
    return true;
}

bool ActionInterval::isDone() const
{
    return _done;
}

void ActionInterval::step(float dt)
{
    if (_firstTick)
    {
        _firstTick = false;
        _elapsed = 0;
    }
    else
    {
        _elapsed += dt;
    }
    
    
    float updateDt = std::max(0.0f,                                  // needed for rewind. elapsed could be negative
                              std::min(1.0f, _elapsed / _duration)
                              );
    
    this->update(updateDt);

    _done = _elapsed >= _duration;
}

void ActionInterval::setAmplitudeRate(float /*amp*/)
{
    // Abstract class needs implementation
    CCASSERT(0, "Subclass should implement this method!");
}

float ActionInterval::getAmplitudeRate()
{
    // Abstract class needs implementation
    CCASSERT(0, "Subclass should implement this method!");

    return 0;
}

void ActionInterval::startWithTarget(Node *target)
{
    FiniteTimeAction::startWithTarget(target);
    _elapsed = 0.0f;
    _firstTick = true;
    _done = false;
}

//
// Sequence
//

Sequence* Sequence::createWithTwoActions(FiniteTimeAction *actionOne, FiniteTimeAction *actionTwo)
{
    Sequence *sequence = new (std::nothrow) Sequence();
    if (sequence && sequence->initWithTwoActions(actionOne, actionTwo))
    {
        sequence->autorelease();
        return sequence;
    }
    
    delete sequence;
    return nullptr;
}

Sequence* Sequence::create(FiniteTimeAction *action1, ...)
{
    va_list params;
    va_start(params, action1);

    Sequence *ret = Sequence::createWithVariableList(action1, params);

    va_end(params);
    
    return ret;
}

Sequence* Sequence::createWithVariableList(FiniteTimeAction *action1, va_list args)
{
    FiniteTimeAction *now;
    FiniteTimeAction *prev = action1;
    bool bOneAction = true;

    while (action1)
    {
        now = va_arg(args, FiniteTimeAction*);
        if (now)
        {
            prev = createWithTwoActions(prev, now);
            bOneAction = false;
        }
        else
        {
            // If only one action is added to Sequence, make up a Sequence by adding a simplest finite time action.
            if (bOneAction)
            {
                prev = createWithTwoActions(prev, ExtraAction::create());
            }
            break;
        }
    }
    
    return ((Sequence*)prev);
}

Sequence* Sequence::create(const Vector<FiniteTimeAction*>& arrayOfActions)
{
    Sequence* seq = new (std::nothrow) Sequence;
    
    if (seq && seq->init(arrayOfActions))
    {
        seq->autorelease();
        return seq;
    }
    
    delete seq;
    return nullptr;
}

bool Sequence::init(const Vector<FiniteTimeAction*>& arrayOfActions)
{
    auto count = arrayOfActions.size();
    if (count == 0)
        return false;

    if (count == 1)
        return initWithTwoActions(arrayOfActions.at(0), ExtraAction::create());

    // else size > 1
    auto prev = arrayOfActions.at(0);
    for (int i = 1; i < count-1; ++i)
    {
        prev = createWithTwoActions(prev, arrayOfActions.at(i));
    }

    return initWithTwoActions(prev, arrayOfActions.at(count-1));
}

bool Sequence::initWithTwoActions(FiniteTimeAction *actionOne, FiniteTimeAction *actionTwo)
{
    CCASSERT(actionOne != nullptr, "actionOne can't be nullptr!");
    CCASSERT(actionTwo != nullptr, "actionTwo can't be nullptr!");
    if (actionOne == nullptr || actionTwo == nullptr)
    {
        CCLOG("Sequence::initWithTwoActions error: action is nullptr!!");
        return false;
    }

    float d = actionOne->getDuration() + actionTwo->getDuration();
    ActionInterval::initWithDuration(d);

    _actions[0] = actionOne;
    actionOne->retain();

    _actions[1] = actionTwo;
    actionTwo->retain();

    return true;
}

bool Sequence::isDone() const
{
    // fix issue #17884
    if (dynamic_cast<ActionInstant*>(_actions[1]))
        return (_done && _actions[1]->isDone());
    else
        return _done;
}

Sequence* Sequence::clone() const
{
    // no copy constructor
    if (_actions[0] && _actions[1])
    {
        return Sequence::create(_actions[0]->clone(), _actions[1]->clone(), nullptr);
    } else {
        return nullptr;
    }
}

Sequence::Sequence()
: _split(0)
{
    _actions[0] = nullptr;
    _actions[1] = nullptr;
}

Sequence::~Sequence()
{
    CC_SAFE_RELEASE(_actions[0]);
    CC_SAFE_RELEASE(_actions[1]);
}

void Sequence::startWithTarget(Node *target)
{
    if (target == nullptr)
    {
        CCLOG("Sequence::startWithTarget error: target is nullptr!");
        return;
    }
    if (_actions[0] == nullptr || _actions[1] == nullptr)
    {
        CCLOG("Sequence::startWithTarget error: _actions[0] or _actions[1] is nullptr!");
        return;
    }
    if (_duration > FLT_EPSILON)
        // fix #14936 - FLT_EPSILON (instant action) / very fast duration (0.001) leads to worng split, that leads to call instant action few times
        _split = _actions[0]->getDuration() > FLT_EPSILON ? _actions[0]->getDuration() / _duration : 0;
    
    ActionInterval::startWithTarget(target);
    _last = -1;
}

void Sequence::stop()
{
    // Issue #1305
    if( _last != - 1 && _actions[_last])
    {
        _actions[_last]->stop();
    }

    ActionInterval::stop();
}

void Sequence::update(float t)
{
    int found = 0;
    float new_t = 0.0f;

    if( t < _split )
    {
        // action[0]
        found = 0;
        if( _split != 0 )
            new_t = t / _split;
        else
            new_t = 1;

    }
    else
    {
        // action[1]
        found = 1;
        if ( _split == 1 )
            new_t = 1;
        else
            new_t = (t-_split) / (1 - _split );
    }

    if ( found==1 )
    {
        if( _last == -1 )
        {
            // action[0] was skipped, execute it.
            if (_actions[0] != nullptr)
            {
                _actions[0]->startWithTarget(_target);
                _actions[0]->update(1.0f);
                _actions[0]->stop();
            }
        }
        else if( _last == 0 )
        {
            // switching to action 1. stop action 0.
            if (_actions[0] != nullptr)
            {
                _actions[0]->update(1.0f);
                _actions[0]->stop();
            }
        }
    }
    else if(found==0 && _last==1 )
    {
        // Reverse mode ?
        // FIXME: Bug. this case doesn't contemplate when _last==-1, found=0 and in "reverse mode"
        // since it will require a hack to know if an action is on reverse mode or not.
        // "step" should be overridden, and the "reverseMode" value propagated to inner Sequences.
        if (_actions[1] != nullptr)
        {
            _actions[1]->update(0);
            _actions[1]->stop();
        }
    }
    // Last action found and it is done.
    if( found == _last && (_actions[found] == nullptr || _actions[found]->isDone() ))
    {
        return;
    }

    if (_actions[found] != nullptr)
    {
        // Last action found and it is done
        if( found != _last )
        {
            _actions[found]->startWithTarget(_target);
        }
        _actions[found]->update(new_t);
    }
    
    _last = found;
}

Sequence* Sequence::reverse() const
{
    if (_actions[0] && _actions[1])
        return Sequence::createWithTwoActions(_actions[1]->reverse(), _actions[0]->reverse());
    else
        return nullptr;
}

//
// Repeat
//

Repeat* Repeat::create(FiniteTimeAction *action, unsigned int times)
{
    Repeat* repeat = new (std::nothrow) Repeat();
    if (repeat && repeat->initWithAction(action, times))
    {
        repeat->autorelease();
        return repeat;
    }

    delete repeat;
    return nullptr;
}

bool Repeat::initWithAction(FiniteTimeAction *action, unsigned int times)
{
    if (action && ActionInterval::initWithDuration(action->getDuration() * times))
    {
        _times = times;
        _innerAction = action;
        action->retain();

        _actionInstant = dynamic_cast<ActionInstant*>(action) ? true : false;
        //an instant action needs to be executed one time less in the update method since it uses startWithTarget to execute the action
        // minggo: instant action doesn't execute action in Repeat::startWithTarget(), so comment it.
//        if (_actionInstant) 
//        {
//            _times -=1;
//        }
        _total = 0;

        return true;
    }

    return false;
}

Repeat* Repeat::clone() const
{
    // no copy constructor
    return Repeat::create(_innerAction->clone(), _times);
}

Repeat::~Repeat()
{
    CC_SAFE_RELEASE(_innerAction);
}

void Repeat::startWithTarget(Node *target)
{
    _total = 0;
    _nextDt = _innerAction->getDuration()/_duration;
    ActionInterval::startWithTarget(target);
    _innerAction->startWithTarget(target);
}

void Repeat::stop()
{
    _innerAction->stop();
    ActionInterval::stop();
}

// issue #80. Instead of hooking step:, hook update: since it can be called by any 
// container action like Repeat, Sequence, Ease, etc..
void Repeat::update(float dt)
{
    if (dt >= _nextDt)
    {
        while (dt >= _nextDt && _total < _times)
        {
            _innerAction->update(1.0f);
            _total++;

            _innerAction->stop();
            _innerAction->startWithTarget(_target);
            _nextDt = _innerAction->getDuration()/_duration * (_total+1);
        }

        // fix for issue #1288, incorrect end value of repeat
        if (std::abs(dt - 1.0f) < FLT_EPSILON && _total < _times)
        {
            _innerAction->update(1.0f);
            
            _total++;
        }

        // don't set an instant action back or update it, it has no use because it has no duration
        if (!_actionInstant)
        {
            if (_total == _times)
            {
                _innerAction->stop();
            }
            else
            {
                // issue #390 prevent jerk, use right update
                _innerAction->update(dt - (_nextDt - _innerAction->getDuration()/_duration));
            }
        }
    }
    else
    {
        _innerAction->update(fmodf(dt * _times,1.0f));
    }
}

bool Repeat::isDone() const
{
    return _total == _times;
}

Repeat* Repeat::reverse() const
{
    return Repeat::create(_innerAction->reverse(), _times);
}

//
// RepeatForever
//
RepeatForever::~RepeatForever()
{
    CC_SAFE_RELEASE(_innerAction);
}

RepeatForever *RepeatForever::create(ActionInterval *action)
{
    RepeatForever *ret = new (std::nothrow) RepeatForever();
    if (ret && ret->initWithAction(action))
    {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

bool RepeatForever::initWithAction(ActionInterval *action)
{
    CCASSERT(action != nullptr, "action can't be nullptr!");
    if (action == nullptr)
    {
        CCLOG("RepeatForever::initWithAction error:action is nullptr!");
        return false;
    }
    
    action->retain();
    _innerAction = action;
    
    return true;
}

RepeatForever *RepeatForever::clone() const
{
    // no copy constructor
    return RepeatForever::create(_innerAction->clone());
}

void RepeatForever::startWithTarget(Node* target)
{
    ActionInterval::startWithTarget(target);
    _innerAction->startWithTarget(target);
}

void RepeatForever::step(float dt)
{
    _innerAction->step(dt);
    // only action interval should prevent jerk, issue #17808
    if (_innerAction->isDone() && _innerAction->getDuration() > 0)
    {
        float diff = _innerAction->getElapsed() - _innerAction->getDuration();
        if (diff > _innerAction->getDuration())
            diff = fmodf(diff, _innerAction->getDuration());
        _innerAction->startWithTarget(_target);
        // to prevent jerk. cocos2d-iphone issue #390, 1247
        _innerAction->step(0.0f);
        _innerAction->step(diff);
    }
}

bool RepeatForever::isDone() const
{
    return false;
}

RepeatForever *RepeatForever::reverse() const
{
    return RepeatForever::create(_innerAction->reverse());
}

//
// Spawn
//

Spawn* Spawn::create(FiniteTimeAction *action1, ...)
{
    va_list params;
    va_start(params, action1);

    Spawn *ret = Spawn::createWithVariableList(action1, params);

    va_end(params);
    
    return ret;
}

Spawn* Spawn::createWithVariableList(FiniteTimeAction *action1, va_list args)
{
    FiniteTimeAction *now;
    FiniteTimeAction *prev = action1;
    bool oneAction = true;

    while (action1)
    {
        now = va_arg(args, FiniteTimeAction*);
        if (now)
        {
            prev = createWithTwoActions(prev, now);
            oneAction = false;
        }
        else
        {
            // If only one action is added to Spawn, make up a Spawn by adding a simplest finite time action.
            if (oneAction)
            {
                prev = createWithTwoActions(prev, ExtraAction::create());
            }
            break;
        }
    }

    return ((Spawn*)prev);
}

Spawn* Spawn::create(const Vector<FiniteTimeAction*>& arrayOfActions)
{
    Spawn* ret = new (std::nothrow) Spawn;
    
    if (ret && ret->init(arrayOfActions))
    {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

Spawn* Spawn::createWithTwoActions(FiniteTimeAction *action1, FiniteTimeAction *action2)
{
    Spawn *spawn = new (std::nothrow) Spawn();
    if (spawn && spawn->initWithTwoActions(action1, action2))
    {
        spawn->autorelease();
        return spawn;
    }
    
    delete spawn;
    return nullptr;
}

bool Spawn::init(const Vector<FiniteTimeAction*>& arrayOfActions)
{
    auto count = arrayOfActions.size();
    
    if (count == 0)
        return false;
    
    if (count == 1)
        return initWithTwoActions(arrayOfActions.at(0), ExtraAction::create());
    
    // else count > 1
    auto prev = arrayOfActions.at(0);
    for (int i = 1; i < count-1; ++i)
    {
        prev = createWithTwoActions(prev, arrayOfActions.at(i));
    }
    
    return initWithTwoActions(prev, arrayOfActions.at(count-1));
}

bool Spawn::initWithTwoActions(FiniteTimeAction *action1, FiniteTimeAction *action2)
{
    CCASSERT(action1 != nullptr, "action1 can't be nullptr!");
    CCASSERT(action2 != nullptr, "action2 can't be nullptr!");
    if (action1 == nullptr || action2 == nullptr)
    {
        CCLOG("Spawn::initWithTwoActions error: action is nullptr!");
        return false;
    }

    bool ret = false;

    float d1 = action1->getDuration();
    float d2 = action2->getDuration();

    if (ActionInterval::initWithDuration(MAX(d1, d2)))
    {
        _one = action1;
        _two = action2;

        if (d1 > d2)
        {
            _two = Sequence::createWithTwoActions(action2, DelayTime::create(d1 - d2));
        } 
        else if (d1 < d2)
        {
            _one = Sequence::createWithTwoActions(action1, DelayTime::create(d2 - d1));
        }

        _one->retain();
        _two->retain();

        ret = true;
    }

    return ret;
}

Spawn* Spawn::clone() const
{
    // no copy constructor
    if (_one && _two)
        return Spawn::createWithTwoActions(_one->clone(), _two->clone());
    else
        return nullptr;
}

Spawn::Spawn()
: _one(nullptr)
, _two(nullptr)
{
    
}

Spawn::~Spawn()
{
    CC_SAFE_RELEASE(_one);
    CC_SAFE_RELEASE(_two);
}

void Spawn::startWithTarget(Node *target)
{
    if (target == nullptr)
    {
        CCLOG("Spawn::startWithTarget error: target is nullptr!");
        return;
    }
    if (_one == nullptr || _two == nullptr)
    {
        CCLOG("Spawn::startWithTarget error: _one or _two is nullptr!");
        return;
    }
    
    ActionInterval::startWithTarget(target);
    _one->startWithTarget(target);
    _two->startWithTarget(target);
}

void Spawn::stop()
{
    if (_one)
        _one->stop();

    if (_two)
        _two->stop();

    ActionInterval::stop();
}

void Spawn::update(float time)
{
    if (_one)
    {
        _one->update(time);
    }
    if (_two)
    {
        _two->update(time);
    }
}

Spawn* Spawn::reverse() const
{
    if (_one && _two)
        return Spawn::createWithTwoActions(_one->reverse(), _two->reverse());
    
    return nullptr;
}

//
// RotateTo
//

RotateTo* RotateTo::create(float duration, float dstAngle)
{
    RotateTo* rotateTo = new (std::nothrow) RotateTo();
    if (rotateTo && rotateTo->initWithDuration(duration, dstAngle))
    {
        rotateTo->autorelease();
        return rotateTo;
    }
    
    delete rotateTo;
    return nullptr;
}

RotateTo::RotateTo()
{
}

bool RotateTo::initWithDuration(float duration, float dstAngle)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _dstAngle = dstAngle;
        
        return true;
    }
    
    return false;
}

RotateTo* RotateTo::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) RotateTo();
    a->initWithDuration(_duration, _dstAngle);
    a->autorelease();
    return a;
}

void RotateTo::calculateAngles(float &startAngle, float &diffAngle, float dstAngle)
{
    if (startAngle > 0)
    {
        startAngle = fmodf(startAngle, 360.0f);
    }
    else
    {
        startAngle = fmodf(startAngle, -360.0f);
    }

    diffAngle = dstAngle - startAngle;
    if (diffAngle > 180)
    {
        diffAngle -= 360;
    }
    if (diffAngle < -180)
    {
        diffAngle += 360;
    }
}

void RotateTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    
    _startAngle = _target->getRotation();

    calculateAngles(_startAngle, _diffAngle, _dstAngle);
}

void RotateTo::update(float time)
{
    if (_target)
    {
        _target->setRotation(_startAngle + _diffAngle * time);
    }
}

RotateTo *RotateTo::reverse() const
{
    CCASSERT(false, "RotateTo doesn't support the 'reverse' method");
    return nullptr;
}

//
// RotateBy
//

RotateBy* RotateBy::create(float duration, float deltaAngle)
{
    RotateBy *rotateBy = new (std::nothrow) RotateBy();
    if (rotateBy && rotateBy->initWithDuration(duration, deltaAngle))
    {
        rotateBy->autorelease();
        return rotateBy;
    }
    
    delete rotateBy;
    return nullptr;
}

RotateBy::RotateBy()
{
}

bool RotateBy::initWithDuration(float duration, float deltaAngle)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _deltaAngle = deltaAngle;
        return true;
    }

    return false;
}


RotateBy* RotateBy::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) RotateBy();
    a->initWithDuration(_duration, _deltaAngle);
    a->autorelease();
    return a;
}

void RotateBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _startAngle = target->getRotation();
}

void RotateBy::update(float time)
{
    // FIXME: shall I add % 360
    if (_target)
    {
        _target->setRotation(_startAngle + _deltaAngle * time);	
    }
}

RotateBy* RotateBy::reverse() const
{
    return RotateBy::create(_duration, -_deltaAngle);
}

//
// MoveBy
//

MoveBy* MoveBy::create(float duration, const Vec2& deltaPosition)
{
    return MoveBy::create(duration, Vec3(deltaPosition.x, deltaPosition.y, 0));
}

MoveBy* MoveBy::create(float duration, const Vec3 &deltaPosition)
{
    MoveBy *ret = new (std::nothrow) MoveBy();
    
    if (ret && ret->initWithDuration(duration, deltaPosition))
    {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

bool MoveBy::initWithDuration(float duration, const Vec2& deltaPosition)
{
    return MoveBy::initWithDuration(duration, Vec3(deltaPosition.x, deltaPosition.y, 0));
}

bool MoveBy::initWithDuration(float duration, const Vec3& deltaPosition)
{
    bool ret = false;
    
    if (ActionInterval::initWithDuration(duration))
    {
        _positionDelta = deltaPosition;
        _is3D = true;
        ret = true;
    }
    
    return ret;
}

MoveBy* MoveBy::clone() const
{
    // no copy constructor
    return MoveBy::create(_duration, _positionDelta);
}

void MoveBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _previousPosition = _startPosition = target->getPosition3D();
}

MoveBy* MoveBy::reverse() const
{
    return MoveBy::create(_duration, -_positionDelta);
}

void MoveBy::update(float t)
{
    if (_target)
    {
#if CC_ENABLE_STACKABLE_ACTIONS
        Vec3 currentPos = _target->getPosition3D();
        Vec3 diff = currentPos - _previousPosition;
        _startPosition = _startPosition + diff;
        Vec3 newPos =  _startPosition + (_positionDelta * t);
        _target->setPosition3D(newPos);
        _previousPosition = newPos;
#else
        _target->setPosition3D(_startPosition + _positionDelta * t);
#endif // CC_ENABLE_STACKABLE_ACTIONS
    }
}

//
// MoveTo
//

MoveTo* MoveTo::create(float duration, const Vec2& position)
{
    return MoveTo::create(duration, Vec3(position.x, position.y, 0));
}

MoveTo* MoveTo::create(float duration, const Vec3& position)
{
    MoveTo *ret = new (std::nothrow) MoveTo();
    
    if (ret && ret->initWithDuration(duration, position))
    {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

bool MoveTo::initWithDuration(float duration, const Vec2& position)
{
    return initWithDuration(duration, Vec3(position.x, position.y, 0));
}

bool MoveTo::initWithDuration(float duration, const Vec3& position)
{
    bool ret = false;
    
    if (ActionInterval::initWithDuration(duration))
    {
        _endPosition = position;
        ret = true;
    }
    
    return ret;
}

MoveTo* MoveTo::clone() const
{
    // no copy constructor
    return MoveTo::create(_duration, _endPosition);
}

void MoveTo::startWithTarget(Node *target)
{
    MoveBy::startWithTarget(target);
    _positionDelta = _endPosition - target->getPosition3D();
}

MoveTo* MoveTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in MoveTo");
    return nullptr;
}

ResizeTo* ResizeTo::create(float duration, const cocos2d::CSize& final_size)
{
    ResizeTo *ret = new (std::nothrow) ResizeTo();
    
    if (ret)
    {
        if (ret->initWithDuration(duration, final_size))
        {
            ret->autorelease();
        } 
        else
        {
            delete ret;
            ret = nullptr;
        }
    }
    
    return ret;
}

ResizeTo* ResizeTo::clone() const
{
    // no copy constructor
    ResizeTo* a = new (std::nothrow) ResizeTo();
    a->initWithDuration(_duration, _finalSize);
    a->autorelease();

    return a;
}

void ResizeTo::startWithTarget(cocos2d::Node* target)
{
    ActionInterval::startWithTarget(target);
    _initialSize = target->getContentSize();
    _sizeDelta = _finalSize - _initialSize;
}

void ResizeTo::update(float time)
{
    if (_target)
    {
        auto new_size = _initialSize + (_sizeDelta * time);
        _target->setContentSize(new_size);
    }
}

bool ResizeTo::initWithDuration(float duration, const cocos2d::CSize& final_size)
{
    if (cocos2d::ActionInterval::initWithDuration(duration))
    {
        _finalSize = final_size;
        return true;
    }

    return false;
}

//
// ResizeBy
//

ResizeBy* ResizeBy::create(float duration, const cocos2d::CSize& deltaSize)
{
    ResizeBy *ret = new (std::nothrow) ResizeBy();
    
    if (ret)
    {
        if (ret->initWithDuration(duration, deltaSize))
        {
            ret->autorelease();
        } 
        else
        {
              delete ret;
              ret = nullptr;
        }
    }
    
    return ret;
}

ResizeBy* ResizeBy::clone() const
{
    // no copy constructor
    auto a = new (std::nothrow) ResizeBy();
    a->initWithDuration(_duration, _sizeDelta);
    a->autorelease();
    return a;
}

void ResizeBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _previousSize = _startSize = target->getContentSize();
}

ResizeBy* ResizeBy::reverse() const
{
    cocos2d::CSize newSize(-_sizeDelta.width, -_sizeDelta.height);
    return ResizeBy::create(_duration, newSize);
}

void ResizeBy::update(float t)
{
    if (_target)
    {
        _target->setContentSize(_startSize + (_sizeDelta * t));
    }
}

bool ResizeBy::initWithDuration(float duration, const cocos2d::CSize& deltaSize)
{
    bool ret = false;
    
    if (ActionInterval::initWithDuration(duration))
    {
        _sizeDelta = deltaSize;
        ret = true;
    }
    
    return ret;
}

//
// JumpBy
//

JumpBy* JumpBy::create(float duration, const Vec2& position, float height, int jumps)
{
    JumpBy *jumpBy = new (std::nothrow) JumpBy();
    if (jumpBy && jumpBy->initWithDuration(duration, position, height, jumps))
    {
        jumpBy->autorelease();
        return jumpBy;
    }
    
    delete jumpBy;
    return nullptr;
}

bool JumpBy::initWithDuration(float duration, const Vec2& position, float height, int jumps)
{
    CCASSERT(jumps>=0, "Number of jumps must be >= 0");
    if (jumps < 0)
    {
        CCLOG("JumpBy::initWithDuration error: Number of jumps must be >= 0");
        return false;
    }
    
    if (ActionInterval::initWithDuration(duration) && jumps>=0)
    {
        _delta = position;
        _height = height;
        _jumps = jumps;

        return true;
    }

    return false;
}

JumpBy* JumpBy::clone() const
{
    // no copy constructor
    return JumpBy::create(_duration, _delta, _height, _jumps);
}

void JumpBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _previousPos = _startPosition = target->getPosition();
}

void JumpBy::update(float t)
{
    // parabolic jump (since v0.8.2)
    if (_target)
    {
        float frac = fmodf( t * _jumps, 1.0f );
        float y = _height * 4 * frac * (1 - frac);
        y += _delta.y * t;

        float x = _delta.x * t;
#if CC_ENABLE_STACKABLE_ACTIONS
        Vec2 currentPos = _target->getPosition();

        Vec2 diff = currentPos - _previousPos;
        _startPosition = diff + _startPosition;

        Vec2 newPos = _startPosition + Vec2(x,y);
        _target->setPosition(newPos);

        _previousPos = newPos;
#else
        _target->setPosition(_startPosition + Vec2(x,y));
#endif // !CC_ENABLE_STACKABLE_ACTIONS
    }
}

JumpBy* JumpBy::reverse() const
{
    return JumpBy::create(_duration, Vec2(-_delta.x, -_delta.y),
        _height, _jumps);
}

//
// JumpTo
//

JumpTo* JumpTo::create(float duration, const Vec2& position, float height, int jumps)
{
    JumpTo *jumpTo = new (std::nothrow) JumpTo();
    if (jumpTo && jumpTo->initWithDuration(duration, position, height, jumps))
    {
        jumpTo->autorelease();
        return jumpTo;
    }
    
    delete jumpTo;
    return nullptr;
}

bool JumpTo::initWithDuration(float duration, const Vec2& position, float height, int jumps)
{
    CCASSERT(jumps>=0, "Number of jumps must be >= 0");
    if (jumps < 0)
    {
        CCLOG("JumpTo::initWithDuration error:Number of jumps must be >= 0");
        return false;
    }

    if (ActionInterval::initWithDuration(duration) && jumps>=0)
    {
        _endPosition = position;
        _height = height;
        _jumps = jumps;

        return true;
    }

    return false;
}

JumpTo* JumpTo::clone() const
{
    // no copy constructor
    return JumpTo::create(_duration, _endPosition, _height, _jumps);
}

JumpTo* JumpTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in JumpTo");
    return nullptr;
}

void JumpTo::startWithTarget(Node *target)
{
    JumpBy::startWithTarget(target);
    _delta.set(_endPosition.x - _startPosition.x, _endPosition.y - _startPosition.y);
}

// Bezier cubic formula:
//    ((1 - t) + t)3 = 1 
// Expands to ...
//   (1 - t)3 + 3t(1-t)2 + 3t2(1 - t) + t3 = 1 
static inline float bezierat( float a, float b, float c, float d, float t )
{
    return (powf(1-t,3) * a + 
            3*t*(powf(1-t,2))*b + 
            3*powf(t,2)*(1-t)*c +
            powf(t,3)*d );
}

//
// BezierBy
//

BezierBy* BezierBy::create(float t, const ccBezierConfig& c)
{
    BezierBy *bezierBy = new (std::nothrow) BezierBy();
    if (bezierBy && bezierBy->initWithDuration(t, c))
    {
        bezierBy->autorelease();
        return bezierBy;
    }
    
    delete bezierBy;
    return nullptr;
}

bool BezierBy::initWithDuration(float t, const ccBezierConfig& c)
{
    if (ActionInterval::initWithDuration(t))
    {
        _config = c;
        return true;
    }

    return false;
}

void BezierBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _previousPosition = _startPosition = target->getPosition();
}

BezierBy* BezierBy::clone() const
{
    // no copy constructor
    return BezierBy::create(_duration, _config);
}

void BezierBy::update(float time)
{
    if (_target)
    {
        float xa = 0;
        float xb = _config.controlPoint_1.x;
        float xc = _config.controlPoint_2.x;
        float xd = _config.endPosition.x;

        float ya = 0;
        float yb = _config.controlPoint_1.y;
        float yc = _config.controlPoint_2.y;
        float yd = _config.endPosition.y;

        float x = bezierat(xa, xb, xc, xd, time);
        float y = bezierat(ya, yb, yc, yd, time);

#if CC_ENABLE_STACKABLE_ACTIONS
        Vec2 currentPos = _target->getPosition();
        Vec2 diff = currentPos - _previousPosition;
        _startPosition = _startPosition + diff;

        Vec2 newPos = _startPosition + Vec2(x,y);
        _target->setPosition(newPos);

        _previousPosition = newPos;
#else
        _target->setPosition( _startPosition + Vec2(x,y));
#endif // !CC_ENABLE_STACKABLE_ACTIONS
    }
}

BezierBy* BezierBy::reverse() const
{
    ccBezierConfig r;

    r.endPosition = -_config.endPosition;
    r.controlPoint_1 = _config.controlPoint_2 + (-_config.endPosition);
    r.controlPoint_2 = _config.controlPoint_1 + (-_config.endPosition);

    BezierBy *action = BezierBy::create(_duration, r);
    return action;
}

//
// BezierTo
//

BezierTo* BezierTo::create(float t, const ccBezierConfig& c)
{
    BezierTo *bezierTo = new (std::nothrow) BezierTo();
    if (bezierTo && bezierTo->initWithDuration(t, c))
    {
        bezierTo->autorelease();
        return bezierTo;
    }
    
    delete bezierTo;
    return nullptr;
}

bool BezierTo::initWithDuration(float t, const ccBezierConfig &c)
{
    if (ActionInterval::initWithDuration(t))
    {
        _toConfig = c;
        return true;
    }
    
    return false;
}

BezierTo* BezierTo::clone() const
{
    // no copy constructor
    return BezierTo::create(_duration, _toConfig);
}

void BezierTo::startWithTarget(Node *target)
{
    BezierBy::startWithTarget(target);
    _config.controlPoint_1 = _toConfig.controlPoint_1 - _startPosition;
    _config.controlPoint_2 = _toConfig.controlPoint_2 - _startPosition;
    _config.endPosition = _toConfig.endPosition - _startPosition;
}

BezierTo* BezierTo::reverse() const
{
    CCASSERT(false, "CCBezierTo doesn't support the 'reverse' method");
    return nullptr;
}

//
// ScaleTo
//
ScaleTo* ScaleTo::create(float duration, float s)
{
    ScaleTo *scaleTo = new (std::nothrow) ScaleTo();
    if (scaleTo && scaleTo->initWithDuration(duration, s))
    {
        scaleTo->autorelease();
        return  scaleTo;
    }
    
    delete scaleTo;
    return nullptr;
}

ScaleTo* ScaleTo::create(float duration, float sx, float sy)
{
    ScaleTo *scaleTo = new (std::nothrow) ScaleTo();
    if (scaleTo && scaleTo->initWithDuration(duration, sx, sy))
    {
        scaleTo->autorelease();
        return scaleTo;
    }
    
    delete scaleTo;
    return nullptr;
}

bool ScaleTo::initWithDuration(float duration, float s)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _endScaleX = s;
        _endScaleY = s;

        return true;
    }

    return false;
}

bool ScaleTo::initWithDuration(float duration, float sx, float sy)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _endScaleX = sx;
        _endScaleY = sy;

        return true;
    }

    return false;
}

ScaleTo* ScaleTo::clone() const
{
    // no copy constructor
    return ScaleTo::create(_duration, _endScaleX, _endScaleY);
}

ScaleTo* ScaleTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in ScaleTo");
    return nullptr;
}

void ScaleTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _startScaleX = target->getScaleX();
    _startScaleY = target->getScaleY();
    _deltaX = _endScaleX - _startScaleX;
    _deltaY = _endScaleY - _startScaleY;
}

void ScaleTo::update(float time)
{
    if (_target)
    {
        _target->setScaleX(_startScaleX + _deltaX * time);
        _target->setScaleY(_startScaleY + _deltaY * time);
    }
}

//
// ScaleBy
//

ScaleBy* ScaleBy::create(float duration, float s)
{
    ScaleBy *scaleBy = new (std::nothrow) ScaleBy();
    if (scaleBy && scaleBy->initWithDuration(duration, s))
    {
        scaleBy->autorelease();
        return scaleBy;
    }
    
    delete scaleBy;
    return nullptr;
}

ScaleBy* ScaleBy::create(float duration, float sx, float sy)
{
    ScaleBy *scaleBy = new (std::nothrow) ScaleBy();
    if (scaleBy && scaleBy->initWithDuration(duration, sx, sy))
    {
        scaleBy->autorelease();
        return scaleBy;
    }
    
    delete scaleBy;
    return nullptr;
}

ScaleBy* ScaleBy::clone() const
{
    // no copy constructor
    return ScaleBy::create(_duration, _endScaleX, _endScaleY);
}

void ScaleBy::startWithTarget(Node *target)
{
    ScaleTo::startWithTarget(target);
    _deltaX = _startScaleX * _endScaleX - _startScaleX;
    _deltaY = _startScaleY * _endScaleY - _startScaleY;
}

ScaleBy* ScaleBy::reverse() const
{
    return ScaleBy::create(_duration, 1 / _endScaleX, 1 / _endScaleY);
}

//
// Blink
//

Blink* Blink::create(float duration, int blinks)
{
    Blink *blink = new (std::nothrow) Blink();
    if (blink && blink->initWithDuration(duration, blinks))
    {
        blink->autorelease();
        return blink;
    }

    delete blink;
    return nullptr;
}

bool Blink::initWithDuration(float duration, int blinks)
{
    CCASSERT(blinks>=0, "blinks should be >= 0");
    if (blinks < 0)
    {
        CCLOG("Blink::initWithDuration error:blinks should be >= 0");
        return false;
    }
    
    if (ActionInterval::initWithDuration(duration) && blinks>=0)
    {
        _times = blinks;
        return true;
    }

    return false;
}

void Blink::stop()
{
    if (nullptr != _target)
        _target->setVisible(_originalState);
    ActionInterval::stop();
}

void Blink::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _originalState = target->isVisible();
}

Blink* Blink::clone() const
{
    // no copy constructor
    return Blink::create(_duration, _times);
}

void Blink::update(float time)
{
    if (_target && ! isDone())
    {
        float slice = 1.0f / _times;
        float m = fmodf(time, slice);
        _target->setVisible(m > slice / 2 ? true : false);
    }
}

Blink* Blink::reverse() const
{
    return Blink::create(_duration, _times);
}

//
// FadeIn
//

FadeIn* FadeIn::create(float d)
{
    FadeIn* action = new (std::nothrow) FadeIn();
    if (action && action->initWithDuration(d,255.0f))
    {
        action->autorelease();
        return action;
    }

    delete action;
    return nullptr;
}

FadeIn* FadeIn::clone() const
{
    // no copy constructor
    return FadeIn::create(_duration);
}

void FadeIn::setReverseAction(cocos2d::FadeTo *ac)
{
    _reverseAction = ac;
}


FadeTo* FadeIn::reverse() const
{
    auto action = FadeOut::create(_duration);
    action->setReverseAction(const_cast<FadeIn*>(this));
    return action;
    
}

void FadeIn::startWithTarget(cocos2d::Node *target)
{
    ActionInterval::startWithTarget(target);
    
    if (nullptr != _reverseAction)
        this->_toOpacity = this->_reverseAction->_fromOpacity;
    else
        _toOpacity = 255.0f;
    
    if (target)
        _fromOpacity = target->getOpacity();
}

//
// FadeOut
//

FadeOut* FadeOut::create(float d)
{
    FadeOut* action = new (std::nothrow) FadeOut();
    if (action && action->initWithDuration(d,0.0f))
    {
        action->autorelease();
        return action;
    }
    
    delete action;
    return nullptr;
}

FadeOut* FadeOut::clone() const
{
    // no copy constructor
    return FadeOut::create(_duration);
}

void FadeOut::startWithTarget(cocos2d::Node *target)
{
    ActionInterval::startWithTarget(target);
    
    if (nullptr != _reverseAction)
        _toOpacity = _reverseAction->_fromOpacity;
    else
        _toOpacity = 0.0f;
    
    if (target)
        _fromOpacity = target->getOpacity();
}

void FadeOut::setReverseAction(cocos2d::FadeTo *ac)
{
    _reverseAction = ac;
}


FadeTo* FadeOut::reverse() const
{
    auto action = FadeIn::create(_duration);
    action->setReverseAction(const_cast<FadeOut*>(this));
    return action;
}

//
// FadeTo
//

FadeTo* FadeTo::create(float duration, GLubyte opacity)
{
    FadeTo *fadeTo = new (std::nothrow) FadeTo();
    if (fadeTo && fadeTo->initWithDuration(duration, opacity))
    {
        fadeTo->autorelease();
        return fadeTo;
    }
    
    delete fadeTo;
    return nullptr;
}

bool FadeTo::initWithDuration(float duration, GLubyte opacity)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _toOpacity = opacity;
        return true;
    }

    return false;
}

FadeTo* FadeTo::clone() const
{
    // no copy constructor
    return FadeTo::create(_duration, _toOpacity);
}

FadeTo* FadeTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in FadeTo");
    return nullptr;
}

void FadeTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);

    if (target)
    {
        _fromOpacity = target->getOpacity();
    }
}

void FadeTo::update(float time)
{
    if (_target)
    {
        _target->setOpacity((GLubyte)(_fromOpacity + (_toOpacity - _fromOpacity) * time));
    }
}

//
// TintTo
//
TintTo* TintTo::create(float duration, GLubyte red, GLubyte green, GLubyte blue)
{
    TintTo *tintTo = new (std::nothrow) TintTo();
    if (tintTo && tintTo->initWithDuration(duration, red, green, blue))
    {
        tintTo->autorelease();
        return tintTo;
    }
    
    delete tintTo;
    return nullptr;
}

TintTo* TintTo::create(float duration, const Color3B& color)
{
    return create(duration, color.r, color.g, color.b);
}

bool TintTo::initWithDuration(float duration, GLubyte red, GLubyte green, GLubyte blue)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _to = Color3B(red, green, blue);
        return true;
    }

    return false;
}

TintTo* TintTo::clone() const
{
    // no copy constructor
    return TintTo::create(_duration, _to.r, _to.g, _to.b);
}

TintTo* TintTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in TintTo");
    return nullptr;
}

void TintTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    if (_target)
    {
        _from = _target->getColor();
    }
}

void TintTo::update(float time)
{
    if (_target)
    {
        _target->setColor(Color3B(GLubyte(_from.r + (_to.r - _from.r) * time),
            (GLubyte)(_from.g + (_to.g - _from.g) * time),
            (GLubyte)(_from.b + (_to.b - _from.b) * time)));
    }
}

//
// TintBy
//

TintBy* TintBy::create(float duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue)
{
    TintBy *tintBy = new (std::nothrow) TintBy();
    if (tintBy && tintBy->initWithDuration(duration, deltaRed, deltaGreen, deltaBlue))
    {
        tintBy->autorelease();
        return tintBy;
    }
    
    delete tintBy;
    return nullptr;
}

bool TintBy::initWithDuration(float duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _deltaR = deltaRed;
        _deltaG = deltaGreen;
        _deltaB = deltaBlue;

        return true;
    }

    return false;
}

TintBy* TintBy::clone() const
{
    // no copy constructor
    return TintBy::create(_duration, _deltaR, _deltaG, _deltaB);
}

void TintBy::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);

    if (target)
    {
        Color3B color = target->getColor();
        _fromR = color.r;
        _fromG = color.g;
        _fromB = color.b;
    }    
}

void TintBy::update(float time)
{
    if (_target)
    {
        _target->setColor(Color3B((GLubyte)(_fromR + _deltaR * time),
            (GLubyte)(_fromG + _deltaG * time),
            (GLubyte)(_fromB + _deltaB * time)));
    }    
}

TintBy* TintBy::reverse() const
{
    return TintBy::create(_duration, -_deltaR, -_deltaG, -_deltaB);
}

//
// DelayTime
//
DelayTime* DelayTime::create(float d)
{
    DelayTime* action = new (std::nothrow) DelayTime();
    if (action && action->initWithDuration(d))
    {
        action->autorelease();
        return action;
    }
    
    delete action;
    return nullptr;
}

DelayTime* DelayTime::clone() const
{
    // no copy constructor
    return DelayTime::create(_duration);
}

void DelayTime::update(float /*time*/)
{
    return;
}

DelayTime* DelayTime::reverse() const
{
    return DelayTime::create(_duration);
}

//
// ReverseTime
//

ReverseTime* ReverseTime::create(FiniteTimeAction *action)
{
    // casting to prevent warnings
    ReverseTime *reverseTime = new (std::nothrow) ReverseTime();
    if (reverseTime && reverseTime->initWithAction( action->clone() ))
    {
        reverseTime->autorelease();
        return reverseTime;
    }
    
    delete reverseTime;
    return nullptr;
}

bool ReverseTime::initWithAction(FiniteTimeAction *action)
{
    CCASSERT(action != nullptr, "action can't be nullptr!");
    CCASSERT(action != _other, "action doesn't equal to _other!");
    if (action == nullptr || action == _other)
    {
        CCLOG("ReverseTime::initWithAction error: action is null or action equal to _other");
        return false;
    }

    if (ActionInterval::initWithDuration(action->getDuration()))
    {
        // Don't leak if action is reused
        CC_SAFE_RELEASE(_other);

        _other = action;
        action->retain();

        return true;
    }

    return false;
}

ReverseTime* ReverseTime::clone() const
{
    // no copy constructor
    return ReverseTime::create(_other->clone());
}

ReverseTime::ReverseTime() : _other(nullptr)
{

}

ReverseTime::~ReverseTime()
{
    CC_SAFE_RELEASE(_other);
}

void ReverseTime::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _other->startWithTarget(target);
}

void ReverseTime::stop()
{
    _other->stop();
    ActionInterval::stop();
}

void ReverseTime::update(float time)
{
    if (_other)
    {
        _other->update(1 - time);
    }
}

ReverseTime* ReverseTime::reverse() const
{
    // FIXME: This looks like a bug
    return (ReverseTime*)_other->clone();
}

// TargetedAction

TargetedAction::TargetedAction()
: _action(nullptr)
, _forcedTarget(nullptr)
{

}

TargetedAction::~TargetedAction()
{
    CC_SAFE_RELEASE(_forcedTarget);
    CC_SAFE_RELEASE(_action);
}

TargetedAction* TargetedAction::create(Node* target, FiniteTimeAction* action)
{
    TargetedAction* p = new (std::nothrow) TargetedAction();
    if (p && p->initWithTarget(target, action))
    {
        p->autorelease();
        return p;
    }
    
    delete p;
    return nullptr;
}


bool TargetedAction::initWithTarget(Node* target, FiniteTimeAction* action)
{
    if(ActionInterval::initWithDuration(action->getDuration()))
    {
        CC_SAFE_RETAIN(target);
        _forcedTarget = target;
        CC_SAFE_RETAIN(action);
        _action = action;
        return true;
    }
    return false;
}

TargetedAction* TargetedAction::clone() const
{
    // no copy constructor
    // win32 : use the _other's copy object.
    return TargetedAction::create(_forcedTarget, _action->clone());
}

TargetedAction* TargetedAction::reverse() const
{
    // just reverse the internal action
    auto a = new (std::nothrow) TargetedAction();
    a->initWithTarget(_forcedTarget, _action->reverse());
    a->autorelease();
    return a;
}

void TargetedAction::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _action->startWithTarget(_forcedTarget);
}

void TargetedAction::stop()
{
    _action->stop();
}

void TargetedAction::update(float time)
{
    _action->update(time);
}

void TargetedAction::setForcedTarget(Node* forcedTarget)
{
    if( _forcedTarget != forcedTarget )
    {
        CC_SAFE_RETAIN(forcedTarget);
        CC_SAFE_RELEASE(_forcedTarget);
        _forcedTarget = forcedTarget;
    }
}

// ActionFloat

ActionFloat* ActionFloat::create(float duration, float from, float to, ActionFloatCallback callback)
{
    auto ref = new (std::nothrow) ActionFloat();
    if (ref && ref->initWithDuration(duration, from, to, callback))
    {
        ref->autorelease();
        return ref;
    }
    
    delete ref;
    return nullptr;
}

bool ActionFloat::initWithDuration(float duration, float from, float to, ActionFloatCallback callback)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _from = from;
        _to = to;
        _callback = callback;
        return true;
    }
    return false;
}

ActionFloat* ActionFloat::clone() const
{
    return ActionFloat::create(_duration, _from, _to, _callback);
}

void ActionFloat::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _delta = _to - _from;
}

void ActionFloat::update(float delta)
{
    float value = _to - _delta * (1 - delta);

    if (_callback)
    {
        // report back value to caller
        _callback(value);
    }
}

ActionFloat* ActionFloat::reverse() const
{
    return ActionFloat::create(_duration, _to, _from, _callback);
}

NS_CC_END
