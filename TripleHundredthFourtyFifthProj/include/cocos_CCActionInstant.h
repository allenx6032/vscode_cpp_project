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

#ifndef __CCINSTANT_ACTION_H__
#define __CCINSTANT_ACTION_H__

#include <functional>
#include "cocos_CCAction.h"

NS_CC_BEGIN

/**
 * @addtogroup actions
 * @{
 */

/** 
@brief Instant actions are immediate actions. They don't have a duration like
the IntervalAction actions.
*/ 
class CC_DLL ActionInstant : public FiniteTimeAction //<NSCopying>
{
public:
    //
    // Overrides
    //
    virtual ActionInstant* clone() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }
    
    virtual ActionInstant * reverse() const override
    {
        CC_ASSERT(0);
        return nullptr;
    }

    virtual bool isDone() const override;
    virtual void step(float dt) override;
    virtual void update(float time) override;
};

/** @brief Show the node
*/
class CC_DLL Show : public ActionInstant
{
public:
    /** Allocates and initializes the action */
    static Show * create();


    //
    // Overrides
    //
    virtual void update(float time) override;
    virtual ActionInstant* reverse() const override;
    virtual Show* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    Show(){}
    virtual ~Show(){}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Show);
};

/** 
@brief Hide the node
*/
class CC_DLL Hide : public ActionInstant
{
public:
    /** Allocates and initializes the action */
    static Hide * create();

    //
    // Overrides
    //
    virtual void update(float time) override;
    virtual ActionInstant* reverse() const override;
    virtual Hide* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    Hide(){}
    virtual ~Hide(){}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Hide);
};

/** @brief Toggles the visibility of a node
*/
class CC_DLL ToggleVisibility : public ActionInstant
{
public:
    /** Allocates and initializes the action */
    static ToggleVisibility * create();

    //
    // Overrides
    //
    virtual void update(float time) override;
    virtual ToggleVisibility* reverse() const override;
    virtual ToggleVisibility* clone() const override;

CC_CONSTRUCTOR_ACCESS:
    ToggleVisibility(){}
    virtual ~ToggleVisibility(){}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ToggleVisibility);
};

/** 
@brief Remove the node
*/
class CC_DLL RemoveSelf : public ActionInstant
{
public:
    /** create the action */
    static RemoveSelf * create(bool isNeedCleanUp = true);

    //
    // Override
    //
    virtual void update(float time) override;
    virtual RemoveSelf* clone() const override;
    virtual RemoveSelf* reverse() const override;
    
CC_CONSTRUCTOR_ACCESS:
    RemoveSelf() : _isNeedCleanUp(true){}
    virtual ~RemoveSelf(){}

    /** init the action */
    bool init(bool isNeedCleanUp);

protected:
    bool _isNeedCleanUp;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(RemoveSelf);
};

/** 
@brief Flips the sprite horizontally
@since v0.99.0
*/
class CC_DLL FlipX : public ActionInstant
{
public:
    /** create the action */
    static FlipX * create(bool x);

    //
    // Overrides
    //
    virtual void update(float time) override;
    virtual FlipX* reverse() const override;
    virtual FlipX* clone() const override;
    
CC_CONSTRUCTOR_ACCESS:
    FlipX() :_flipX(false) {}
    virtual ~FlipX() {}

    /** init the action */
    bool initWithFlipX(bool x);

protected:
    bool    _flipX;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(FlipX);
};

/** 
@brief Flips the sprite vertically
@since v0.99.0
*/
class CC_DLL FlipY : public ActionInstant
{
public:
    /** create the action */
    static FlipY * create(bool y);

    //
    // Overrides
    //
    virtual void update(float time) override;
    virtual FlipY* reverse() const override;
    virtual FlipY* clone() const override;
    
CC_CONSTRUCTOR_ACCESS:
    FlipY() :_flipY(false) {}
    virtual ~FlipY() {}

    /** init the action */
    bool initWithFlipY(bool y);

protected:
    bool    _flipY;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(FlipY);
};

/** @brief Places the node in a certain position
*/
class CC_DLL Place : public ActionInstant //<NSCopying>
{
public:

    /** creates a Place action with a position */
    static Place * create(const Vec2& pos);

    //
    // Overrides
    //
    virtual void update(float time) override;
    virtual Place* reverse() const override;
    virtual Place* clone() const override;
    
CC_CONSTRUCTOR_ACCESS:
    Place(){}
    virtual ~Place(){}

    /** Initializes a Place action with a position */
    bool initWithPosition(const Vec2& pos);

protected:
    Vec2 _position;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Place);
};


/** @brief Calls a 'callback'
*/
class CC_DLL CallFunc : public ActionInstant //<NSCopying>
{
public:
    /** creates the action with the callback of type std::function<void()>.
     This is the preferred way to create the callback.
     * When this funtion bound in js or lua ,the input param will be changed
     * In js: var create(var func, var this, var [data]) or var create(var func)
     * In lua:local create(local funcID)
     */
    static CallFunc * create(const std::function<void()>& func);
    
public:
    /** executes the callback */
    virtual void execute();

    //
    // Overrides
    //
    virtual void update(float time) override;
    virtual CallFunc* reverse() const override;
    virtual CallFunc* clone() const override;
    
CC_CONSTRUCTOR_ACCESS:
    CallFunc()
    : _callFunc(nullptr)
    , _function(nullptr)
    {
    }
    virtual ~CallFunc();
    
    /** initializes the action with the std::function<void()>
     * @js NA
     * @lua NA
     */
    bool initWithFunction(const std::function<void()>& func);

protected:
    union
    {
        SEL_CallFunc    _callFunc;
        SEL_CallFuncN    _callFuncN;
    };
    
    /** function that will be called */
    std::function<void()> _function;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(CallFunc);
};

/** 
@brief Calls a 'callback' with the node as the first argument
N means Node
*/
class CC_DLL CallFuncN : public CallFunc
{
public:
    /** creates the action with the callback of type std::function<void()>.
     This is the preferred way to create the callback.
     */
    static CallFuncN * create(const std::function<void(Node*)>& func);

    //
    // Overrides
    //
    virtual CallFuncN* clone() const override;
    virtual void execute() override;
    
CC_CONSTRUCTOR_ACCESS:
    CallFuncN():_functionN(nullptr){}
    virtual ~CallFuncN(){}

    /** initializes the action with the std::function<void(Node*)> */
    bool initWithFunction(const std::function<void(Node*)>& func);

protected:
    /** function that will be called with the "sender" as the 1st argument */
    std::function<void(Node*)> _functionN;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(CallFuncN);
};

// end of actions group
/// @}

NS_CC_END

#endif //__CCINSTANT_ACTION_H__
