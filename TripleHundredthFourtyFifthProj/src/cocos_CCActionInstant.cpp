/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
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

#include "cocos_CCActionInstant.h"
#include "cocos_CCNode.h"
#include "cocos_CCSprite.h"

NS_CC_BEGIN
//
// InstantAction
//
bool ActionInstant::isDone() const
{
    return true;
}

void ActionInstant::step(float dt) {
    CC_UNUSED_PARAM(dt);
    update(1);
}

void ActionInstant::update(float time) {
    CC_UNUSED_PARAM(time);
    // nothing
}

//
// Show
//

Show* Show::create() 
{
    Show* ret = new (std::nothrow) Show();

    if (ret) {
        ret->autorelease();
    }

    return ret;
}

void Show::update(float time) {
    CC_UNUSED_PARAM(time);
    _target->setVisible(true);
}

ActionInstant* Show::reverse() const
{
    return Hide::create();
}

Show * Show::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) Show();
	a->autorelease();
	return a;
}

//
// Hide
//
Hide * Hide::create() 
{
    Hide *ret = new (std::nothrow) Hide();

    if (ret) {
        ret->autorelease();
    }

    return ret;
}

void Hide::update(float time) {
    CC_UNUSED_PARAM(time);
    _target->setVisible(false);
}

ActionInstant *Hide::reverse() const
{
    return Show::create();
}

Hide * Hide::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) Hide();
	a->autorelease();
	return a;
}

//
// ToggleVisibility
//
ToggleVisibility * ToggleVisibility::create()
{
    ToggleVisibility *ret = new (std::nothrow) ToggleVisibility();

    if (ret)
    {
        ret->autorelease();
    }

    return ret;
}

void ToggleVisibility::update(float time) 
{
    CC_UNUSED_PARAM(time);
    _target->setVisible(!_target->isVisible());
}

ToggleVisibility * ToggleVisibility::reverse() const
{
	return ToggleVisibility::create();
}

ToggleVisibility * ToggleVisibility::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) ToggleVisibility();
	a->autorelease();
	return a;
}

//
// Remove Self
//
RemoveSelf * RemoveSelf::create(bool isNeedCleanUp /*= true*/) 
{
	RemoveSelf *ret = new (std::nothrow) RemoveSelf();

	if (ret && ret->init(isNeedCleanUp)) {
		ret->autorelease();
	}

	return ret;
}

bool RemoveSelf::init(bool isNeedCleanUp) {
	_isNeedCleanUp = isNeedCleanUp;
	return true;
}

void RemoveSelf::update(float time) {
	CC_UNUSED_PARAM(time);
	_target->removeFromParentAndCleanup(_isNeedCleanUp);
}

RemoveSelf *RemoveSelf::reverse() const
{
	return RemoveSelf::create(_isNeedCleanUp);
}

RemoveSelf * RemoveSelf::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) RemoveSelf();
	a->init(_isNeedCleanUp);
	a->autorelease();
	return a;
}

//
// FlipX
//

FlipX *FlipX::create(bool x)
{
    FlipX *ret = new (std::nothrow) FlipX();

    if (ret && ret->initWithFlipX(x)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipX::initWithFlipX(bool x) {
    _flipX = x;
    return true;
}

void FlipX::update(float time) {
    CC_UNUSED_PARAM(time);
    static_cast<Sprite*>(_target)->setFlippedX(_flipX);
}

FlipX* FlipX::reverse() const
{
    return FlipX::create(!_flipX);
}

FlipX * FlipX::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) FlipX();
	a->initWithFlipX(_flipX);
	a->autorelease();
	return a;
}
//
// FlipY
//

FlipY * FlipY::create(bool y)
{
    FlipY *ret = new (std::nothrow) FlipY();

    if (ret && ret->initWithFlipY(y)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FlipY::initWithFlipY(bool y) {
    _flipY = y;
    return true;
}

void FlipY::update(float time) {
    CC_UNUSED_PARAM(time);
    static_cast<Sprite*>(_target)->setFlippedY(_flipY);
}

FlipY* FlipY::reverse() const
{
    return FlipY::create(!_flipY);
}

FlipY * FlipY::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) FlipY();
	a->initWithFlipY(_flipY);
	a->autorelease();
	return a;
}

//
// Place
//

Place* Place::create(const Vec2& pos)
{
    Place *ret = new (std::nothrow) Place();

    if (ret && ret->initWithPosition(pos)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Place::initWithPosition(const Vec2& pos) {
    _position = pos;
    return true;
}

Place * Place::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) Place();
	a->initWithPosition(_position);
	a->autorelease();
	return a;
}

Place * Place::reverse() const
{
	// no reverse, just clone
	return this->clone();
}

void Place::update(float time) {
    CC_UNUSED_PARAM(time);
    _target->setPosition(_position);
}

//
// CallFunc
//

CallFunc * CallFunc::create(const std::function<void()> &func)
{
    CallFunc *ret = new (std::nothrow) CallFunc();

    if (ret && ret->initWithFunction(func) ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CallFunc::initWithFunction(const std::function<void()> &func)
{
	_function = func;
    return true;
}

CallFunc::~CallFunc()
{
}

CallFunc * CallFunc::clone() const
    {
    // no copy constructor
    auto a = new (std::nothrow) CallFunc();
    if( _function ){
        a->initWithFunction(_function);
    }

    a->autorelease();
    return a;
}

CallFunc * CallFunc::reverse() const
{
	// no reverse here, just return a clone
	return this->clone();
}

void CallFunc::update(float time) {
    CC_UNUSED_PARAM(time);
    this->execute();
}

void CallFunc::execute() {
    if( _function ){
        _function();
    }
}

//
// CallFuncN
//

CallFuncN * CallFuncN::create(const std::function<void(Node*)> &func)
{
    auto ret = new (std::nothrow) CallFuncN();

    if (ret && ret->initWithFunction(func) ) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return nullptr;
}

void CallFuncN::execute() {
    if (_functionN) {
        _functionN(_target);
    }
}

bool CallFuncN::initWithFunction(const std::function<void (Node *)> &func)
{
    _functionN = func;
    return true;
}

CallFuncN * CallFuncN::clone() const
{
	// no copy constructor
	auto a = new (std::nothrow) CallFuncN();

    if( _functionN ){
        a->initWithFunction(_functionN);
    }

	a->autorelease();
	return a;
}

NS_CC_END
