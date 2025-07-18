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

#include "cocos_UIWidget.h"
#include "cocos_UILayout.h"
#include "cocos_UIHelper.h"
#include "cocos_CCEventListenerTouch.h"
#include "cocos_CCEventListenerKeyboard.h"
#include "cocos_CCDirector.h"
#include "cocos_CCEventFocus.h"
#include "cocos_CCEventDispatcher.h"
#include "cocos_UILayoutComponent.h"

NS_CC_BEGIN

namespace ui {
    
class Widget::FocusNavigationController
{
    void enableFocusNavigation(bool flag);
    
    FocusNavigationController():
    _keyboardListener(nullptr),
    _firstFocusedWidget(nullptr),
    _enableFocusNavigation(false),
    _keyboardEventPriority(1)
    {
        //no-op
    }
    ~FocusNavigationController();
protected:
    void setFirstFocsuedWidget(Widget* widget);
    
    void onKeypadKeyPressed(EventKeyboard::KeyCode, Event*);
    
    void addKeyboardEventListener();
    void removeKeyboardEventListener();
    
    friend class Widget;
private:
    EventListenerKeyboard* _keyboardListener ;
    Widget* _firstFocusedWidget ;
    bool _enableFocusNavigation ;
    const int _keyboardEventPriority;
};
    
Widget::FocusNavigationController::~FocusNavigationController()
{
    this->removeKeyboardEventListener();
}

void Widget::FocusNavigationController::onKeypadKeyPressed(EventKeyboard::KeyCode  keyCode, Event *event)
{
    if (_enableFocusNavigation && _firstFocusedWidget)
    {
        if (keyCode == EventKeyboard::KeyCode::KEY_DPAD_DOWN)
        {
            _firstFocusedWidget = _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::DOWN, _firstFocusedWidget);
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_DPAD_UP)
        {
            _firstFocusedWidget = _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::UP, _firstFocusedWidget);
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_DPAD_LEFT)
        {
            _firstFocusedWidget = _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::LEFT, _firstFocusedWidget);
        }
        if (keyCode == EventKeyboard::KeyCode::KEY_DPAD_RIGHT)
        {
            _firstFocusedWidget = _firstFocusedWidget->findNextFocusedWidget(Widget::FocusDirection::RIGHT, _firstFocusedWidget);
        }
    }
}

void Widget::FocusNavigationController::enableFocusNavigation(bool flag)
{
    if (_enableFocusNavigation == flag)
        return;
    
    _enableFocusNavigation = flag;
    
    if (flag)
        this->addKeyboardEventListener();
    else
        this->removeKeyboardEventListener();
}

void Widget::FocusNavigationController::setFirstFocsuedWidget(Widget* widget)
{
    _firstFocusedWidget = widget;
}

void Widget::FocusNavigationController::addKeyboardEventListener()
{
    if (nullptr == _keyboardListener)
    {
        _keyboardListener = EventListenerKeyboard::create();
        _keyboardListener->onKeyReleased = CC_CALLBACK_2(Widget::FocusNavigationController::onKeypadKeyPressed, this);
        EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->addEventListenerWithFixedPriority(_keyboardListener, _keyboardEventPriority);
    }
}

void Widget::FocusNavigationController::removeKeyboardEventListener()
{
    if (nullptr != _keyboardListener)
    {
        EventDispatcher* dispatcher = Director::getInstance()->getEventDispatcher();
        dispatcher->removeEventListener(_keyboardListener);
        _keyboardListener = nullptr;
    }
}

Widget* Widget::_focusedWidget = nullptr;
Widget::FocusNavigationController* Widget::_focusNavigationController = nullptr;
    
Widget::Widget():
_unifySize(false),
_enabled(true),
_bright(true),
_touchEnabled(false),
_highlight(false),
_ignoreSize(false),
_propagateTouchEvents(true),
_brightStyle(BrightStyle::NONE),
_sizeType(SizeType::ABSOLUTE),
_positionType(PositionType::ABSOLUTE),
_actionTag(0),
_customSize(Size::ZERO),
_sizePercent(Vec2::ZERO),
_positionPercent(Vec2::ZERO),
_hitted(false),
_touchListener(nullptr),
_touchBeganPosition(Vec2::ZERO),
_touchMovePosition(Vec2::ZERO),
_touchEndPosition(Vec2::ZERO),
_flippedX(false),
_flippedY(false),
_layoutParameterType(LayoutParameter::Type::NONE),
_focused(false),
_focusEnabled(true),
_ccEventCallback(nullptr),
_callbackType(""),
_callbackName("")
{
  
}

Widget::~Widget()
{
    this->cleanupWidget();
}

void Widget::cleanupWidget()
{
    //clean up _touchListener
    _eventDispatcher->removeEventListener(_touchListener);
    CC_SAFE_RELEASE_NULL(_touchListener);
    
    //cleanup focused widget and focus navigation controller
    if (_focusedWidget == this)
    {
        //delete
        CC_SAFE_DELETE(_focusNavigationController);
        _focusedWidget = nullptr;
    }
}

Widget* Widget::create()
{
    Widget* widget = new (std::nothrow) Widget();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}

bool Widget::init()
{
    if (ProtectedNode::init())
    {
        initRenderer();
        setBright(true);
        onFocusChanged = CC_CALLBACK_2(Widget::onFocusChange,this);
        onNextFocusedWidget = nullptr;
        this->setAnchorPoint(Vec2(0.5f, 0.5f));

        ignoreContentAdaptWithSize(true);
        
        return true;
    }
    return false;
}

void Widget::onEnter()
{
    updateSizeAndPosition();
    ProtectedNode::onEnter();
}

void Widget::onExit()
{
    unscheduleUpdate();
    ProtectedNode::onExit();
}

void Widget::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if (_visible)
    {
        adaptRenderers();
        ProtectedNode::visit(renderer, parentTransform, parentFlags);
    }
}

Widget* Widget::getWidgetParent()
{
    return dynamic_cast<Widget*>(getParent());
}

void Widget::setEnabled(bool enabled)
{
    _enabled = enabled;
    setBright(enabled);
}
    
void Widget::initRenderer()
{
}
    
LayoutComponent* Widget::getOrCreateLayoutComponent()
{
    auto layoutComponent = this->getComponent(__LAYOUT_COMPONENT_NAME);
    if (nullptr == layoutComponent)
    {
        LayoutComponent *component = LayoutComponent::create();
        this->addComponent(component);
        layoutComponent = component;
    }
    
    return (LayoutComponent*)layoutComponent;
}
   
void Widget::setContentSize(const cocos2d::Size &contentSize)
{
    ProtectedNode::setContentSize(contentSize);
    
    _customSize = contentSize;
    if (_unifySize)
    {
        //unify Size logic
    }
    else if (_ignoreSize)
    {
        _contentSize = getVirtualRendererSize();
    }
    if (_running)
    {
        Widget* widgetParent = getWidgetParent();
        Size pSize;
        if (widgetParent)
        {
            pSize = widgetParent->getContentSize();
        }
        else
        {
            pSize = _parent->getContentSize();
        }
        float spx = 0.0f;
        float spy = 0.0f;
        if (pSize.width > 0.0f)
        {
            spx = _customSize.width / pSize.width;
        }
        if (pSize.height > 0.0f)
        {
            spy = _customSize.height / pSize.height;
        }
        _sizePercent = Vec2(spx, spy);
    }
    onSizeChanged();
}

void Widget::setSizePercent(const Vec2 &percent)
{
    _sizePercent = percent;
    Size cSize = _customSize;
    if (_running)
    {
        Widget* widgetParent = getWidgetParent();
        if (widgetParent)
        {
            cSize = Size(widgetParent->getContentSize().width * percent.x , widgetParent->getContentSize().height * percent.y);
        }
        else
        {
            cSize = Size(_parent->getContentSize().width * percent.x , _parent->getContentSize().height * percent.y);
        }
    }
    if (_ignoreSize)
    {
        this->setContentSize(getVirtualRendererSize());
    }
    else
    {
        this->setContentSize(cSize);
    }
    _customSize = cSize;
}

void Widget::updateSizeAndPosition()
{
    if (_parent) {
        Size pSize = _parent->getContentSize();
        updateSizeAndPosition(pSize);
    }
}
    
void Widget::updateSizeAndPosition(const cocos2d::Size &parentSize)
{
    switch (_sizeType)
    {
        case SizeType::ABSOLUTE:
        {
            if (_ignoreSize)
            {
                this->setContentSize(getVirtualRendererSize());
            }
            else
            {
                this->setContentSize(_customSize);
            }
            float spx = 0.0f;
            float spy = 0.0f;
            if (parentSize.width > 0.0f)
            {
                spx = _customSize.width / parentSize.width;
            }
            if (parentSize.height > 0.0f)
            {
                spy = _customSize.height / parentSize.height;
            }
            _sizePercent = Vec2(spx, spy);
            break;
        }
        case SizeType::PERCENT:
        {
            Size cSize = Size(parentSize.width * _sizePercent.x , parentSize.height * _sizePercent.y);
            if (_ignoreSize)
            {
                this->setContentSize(getVirtualRendererSize());
            }
            else
            {
                this->setContentSize(cSize);
            }
            _customSize = cSize;
            break;
        }
        default:
            break;
    }
    
    //update position & position percent
    Vec2 absPos = getPosition();
    switch (_positionType)
    {
        case PositionType::ABSOLUTE:
        {
            if (parentSize.width <= 0.0f || parentSize.height <= 0.0f)
            {
                _positionPercent = Vec2::ZERO;
            }
            else
            {
                _positionPercent = Vec2(absPos.x / parentSize.width, absPos.y / parentSize.height);
            }
            break;
        }
        case PositionType::PERCENT:
        {
            absPos = Vec2(parentSize.width * _positionPercent.x, parentSize.height * _positionPercent.y);
            break;
        }
        default:
            break;
    }
    setPosition(absPos);
}

void Widget::setSizeType(SizeType type)
{
    _sizeType = type;
}

Widget::SizeType Widget::getSizeType() const
{
    return _sizeType;
}

void Widget::ignoreContentAdaptWithSize(bool ignore)
{
    if (_unifySize)
    {
        this->setContentSize(_customSize);
        return;
    }
    if (_ignoreSize == ignore)
    {
        return;
    }
    _ignoreSize = ignore;
    if (_ignoreSize)
    {
        Size s = getVirtualRendererSize();
        this->setContentSize(s);
    }
    else
    {
        this->setContentSize(_customSize);
    }
}

bool Widget::isIgnoreContentAdaptWithSize() const
{
    return _ignoreSize;
}
    
const Size& Widget::getCustomSize() const
{
    return _customSize;
}

const Vec2& Widget::getSizePercent()
{
    return _sizePercent;
}

Vec2 Widget::getWorldPosition()const
{
    return convertToWorldSpace(Vec2(_anchorPoint.x * _contentSize.width, _anchorPoint.y * _contentSize.height));
}

Node* Widget::getVirtualRenderer()
{
    return this;
}

void Widget::onSizeChanged()
{
    for (auto& child : getChildren())
    {
        Widget* widgetChild = dynamic_cast<Widget*>(child);
        if (widgetChild)
        {
            widgetChild->updateSizeAndPosition();
        }
    }
}

Size Widget::getVirtualRendererSize() const
{
    return _contentSize;
}
    
void Widget::updateContentSizeWithTextureSize(const cocos2d::Size &size)
{
    if (_unifySize)
    {
        this->setContentSize(size);
        return;
    }
    if (_ignoreSize)
    {
        this->setContentSize(size);
    }
    else
    {
        this->setContentSize(_customSize);
    }
}

void Widget::setTouchEnabled(bool enable)
{
    if (enable == _touchEnabled)
    {
        return;
    }
    _touchEnabled = enable;
    if (_touchEnabled)
    {
        _touchListener = EventListenerTouchOneByOne::create();
        CC_SAFE_RETAIN(_touchListener);
        _touchListener->setSwallowTouches(true);
        _touchListener->onTouchBegan = CC_CALLBACK_2(Widget::onTouchBegan, this);
        _touchListener->onTouchMoved = CC_CALLBACK_2(Widget::onTouchMoved, this);
        _touchListener->onTouchEnded = CC_CALLBACK_2(Widget::onTouchEnded, this);
        _touchListener->onTouchCancelled = CC_CALLBACK_2(Widget::onTouchCancelled, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
    }
    else
    {
        _eventDispatcher->removeEventListener(_touchListener);
        CC_SAFE_RELEASE_NULL(_touchListener);
    }
}

bool Widget::isTouchEnabled() const
{
    return _touchEnabled;
}

bool Widget::isHighlighted() const
{
    return _highlight;
}

void Widget::setHighlighted(bool hilight)
{
    if (hilight == _highlight)
    {
        return;
    }
    _highlight = hilight;
    if (_bright)
    {
        if (_highlight)
        {
            setBrightStyle(BrightStyle::HIGHLIGHT);
        }
        else
        {
            setBrightStyle(BrightStyle::NORMAL);
        }
    }
    else
    {
        onPressStateChangedToDisabled();
    }
}

void Widget::setBright(bool bright)
{
    _bright = bright;
    if (_bright)
    {
        _brightStyle = BrightStyle::NONE;
        setBrightStyle(BrightStyle::NORMAL);
    }
    else
    {
        onPressStateChangedToDisabled();
    }
}

void Widget::setBrightStyle(BrightStyle style)
{
    if (_brightStyle == style)
    {
        return;
    }
    _brightStyle = style;
    switch (_brightStyle)
    {
        case BrightStyle::NORMAL:
            onPressStateChangedToNormal();
            break;
        case BrightStyle::HIGHLIGHT:
            onPressStateChangedToPressed();
            break;
        default:
            break;
    }
}

void Widget::onPressStateChangedToNormal()
{

}

void Widget::onPressStateChangedToPressed()
{

}

void Widget::onPressStateChangedToDisabled()
{

}
    
void Widget::updateChildrenDisplayedRGBA()
{
    this->setColor(this->getColor());
    this->setOpacity(this->getOpacity());
}
    
void Widget::setPropagateTouchEvents(bool isPropagate)
{
    _propagateTouchEvents = isPropagate;
}
    
bool Widget::isPropagateTouchEvents()const
{
    return _propagateTouchEvents;
}
    
void Widget::setSwallowTouches(bool swallow)
{
    if (_touchListener)
    {
        _touchListener->setSwallowTouches(swallow);
    }
}
    
bool Widget::isSwallowTouches()const
{
    if (_touchListener)
    {
        return _touchListener->isSwallowTouches();
    }
    return false;
}

bool Widget::onTouchBegan(Touch *touch, Event *unusedEvent)
{
    _hitted = false;
    if (isVisible(true) && isEnabled(true))
    {
        _touchBeganPosition = touch->getLocation();
        if(hitTest(_touchBeganPosition) && isClippingParentContainsPoint(_touchBeganPosition))
        {
            _hitted = true;
        }
    }
    if (!_hitted)
    {
        return false;
    }
    setHighlighted(true);
    
    /*
     * Propagate touch events to its parents
     */
    if (_propagateTouchEvents)
    {
        this->propagateTouchEvent(TouchEventType::BEGAN, this, touch);
    }
  
    pushDownEvent();
    return true;
}
    
void Widget::propagateTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget *sender, cocos2d::Touch *touch)
{
    // interceptTouchEvent was override by PageView,ListView,ScrollView
    // must find a parent to call, otherwise it will be dealed twice.
    Node *parent = this;
    while (parent) {
        parent = parent->getParent();
        Widget *widget = dynamic_cast<Widget *>(parent);
        if (widget) {
            widget->interceptTouchEvent(event, sender, touch);
            break;
        }
    }
}

void Widget::onTouchMoved(Touch *touch, Event *unusedEvent)
{
    _touchMovePosition = touch->getLocation();
    
    setHighlighted(hitTest(_touchMovePosition));
    
    /*
     * Propagate touch events to its parents
     */
    if (_propagateTouchEvents)
    {
        this->propagateTouchEvent(TouchEventType::MOVED, this, touch);
    }
    
    moveEvent();
}

void Widget::onTouchEnded(Touch *touch, Event *unusedEvent)
{
    _touchEndPosition = touch->getLocation();
    
    /*
     * Propagate touch events to its parents
     */
    if (_propagateTouchEvents)
    {
        this->propagateTouchEvent(TouchEventType::ENDED, this, touch);
    }
    
    bool highlight = _highlight;
    setHighlighted(false);
    
    if (highlight)
    {
        releaseUpEvent();
    }
    else
    {
        cancelUpEvent();
    }
}

void Widget::onTouchCancelled(Touch *touch, Event *unusedEvent)
{
    setHighlighted(false);
    cancelUpEvent();
}

void Widget::pushDownEvent()
{
    this->retain();
    if (_touchEventCallback)
    {
        _touchEventCallback(this, TouchEventType::BEGAN);
    }
    this->release();
}

void Widget::moveEvent()
{
    this->retain();
    if (_touchEventCallback)
    {
        _touchEventCallback(this, TouchEventType::MOVED);
    }
    this->release();
}

void Widget::releaseUpEvent()
{
    this->retain();
    if (_touchEventCallback)
    {
        _touchEventCallback(this, TouchEventType::ENDED);
    }
    
    if (_clickEventListener) {
        _clickEventListener(this);
    }
    this->release();
}

void Widget::cancelUpEvent()
{
    this->retain();
    if (_touchEventCallback)
    {
        _touchEventCallback(this, TouchEventType::CANCELED);
    }
    this->release();
}
    
void Widget::addTouchEventListener(const ccWidgetTouchCallback& callback)
{
    this->_touchEventCallback = callback;
}
    
void Widget::addClickEventListener(const ccWidgetClickCallback &callback)
{
    this->_clickEventListener = callback;
}
    
void Widget::addCCSEventListener(const ccWidgetEventCallback &callback)
{
    this->_ccEventCallback = callback;
}

bool Widget::hitTest(const Vec2 &pt)
{
    Vec2 nsp = convertToNodeSpace(pt);
    Rect bb;
    bb.size = _contentSize;
    if (bb.containsPoint(nsp))
    {
        return true;
    }
    return false;
}

bool Widget::isClippingParentContainsPoint(const Vec2 &pt)
{
    // make sure check not break by non-Layout node.
    Node *parent = this;
    while (parent) {
        parent = parent->getParent();
        Layout *layout = dynamic_cast<Layout*>(parent);
        if (layout && layout->isClippingEnabled()) {
            if (!layout->hitTest(pt)) {
                return false;
            }
        }
    }
    return true;
}

void Widget::interceptTouchEvent(cocos2d::ui::Widget::TouchEventType event, cocos2d::ui::Widget *sender, Touch *touch)
{
    // make sure check not break by non-Widget node.
    Node *parent = this;
    while (parent) {
        parent = parent->getParent();
        Widget *widget = dynamic_cast<Widget *>(parent);
        if (widget) {
            widget->interceptTouchEvent(event, sender, touch);
            break;
        }
    }
}

void Widget::setPosition(const Vec2 &pos)
{
    if (_running)
    {
        Widget* widgetParent = getWidgetParent();
        if (widgetParent)
        {
            Size pSize = widgetParent->getContentSize();
            if (pSize.width <= 0.0f || pSize.height <= 0.0f)
            {
                _positionPercent = Vec2::ZERO;
            }
            else
            {
                _positionPercent = Vec2(pos.x / pSize.width, pos.y / pSize.height);
            }
        }
    }
    ProtectedNode::setPosition(pos);
}

void Widget::setPositionPercent(const Vec2 &percent)
{
    _positionPercent = percent;
    if (_running)
    {
        Widget* widgetParent = getWidgetParent();
        if (widgetParent)
        {
            Size parentSize = widgetParent->getContentSize();
            Vec2 absPos = Vec2(parentSize.width * _positionPercent.x, parentSize.height * _positionPercent.y);
            setPosition(absPos);
        }
    }
}

const Vec2& Widget::getPositionPercent()const{
    return _positionPercent;
}

void Widget::setPositionType(PositionType type)
{
    _positionType = type;
}

Widget::PositionType Widget::getPositionType() const
{
    return _positionType;
}

bool Widget::isBright() const
{
    return _bright;
}

bool Widget::isEnabled(bool checkParent) const
{
    if (!checkParent) {
        return _enabled;
    }
    
    // need check Parent
    if (!_enabled) {
        return false;
    }
    Node *p = _parent;
    while (p) {
        Widget* widget = dynamic_cast<Widget *>(p);
        if (widget && !widget->isEnabled(false)) {
            return false;
        }
        p = p->getParent();
    }
    return true;
}

float Widget::getLeftBoundary() const
{
    return getPosition().x - getAnchorPoint().x * _contentSize.width;
}

float Widget::getBottomBoundary() const
{
    return getPosition().y - getAnchorPoint().y * _contentSize.height;
}

float Widget::getRightBoundary() const
{
    return getLeftBoundary() + _contentSize.width;
}

float Widget::getTopBoundary() const
{
    return getBottomBoundary() + _contentSize.height;
}

const Vec2& Widget::getTouchBeganPosition()const
{
    return _touchBeganPosition;
}

const Vec2& Widget::getTouchMovePosition()const
{
    return _touchMovePosition;
}

const Vec2& Widget::getTouchEndPosition()const
{
    return _touchEndPosition;
}

void Widget::setLayoutParameter(LayoutParameter *parameter)
{
    if (!parameter)
    {
        return;
    }
    _layoutParameterDictionary.insert((int)parameter->getLayoutType(), parameter);
    _layoutParameterType = parameter->getLayoutType();
}

LayoutParameter* Widget::getLayoutParameter()const
{
    return dynamic_cast<LayoutParameter*>(_layoutParameterDictionary.at((int)_layoutParameterType));
}

std::string Widget::getDescription() const
{
    return "Widget";
}

Widget* Widget::clone()
{
    Widget* clonedWidget = createCloneInstance();
    clonedWidget->copyProperties(this);
    clonedWidget->copyClonedWidgetChildren(this);
    return clonedWidget;
}

Widget* Widget::createCloneInstance()
{
    return Widget::create();
}

void Widget::copyClonedWidgetChildren(Widget* model)
{
    auto& modelChildren = model->getChildren();

    for (auto& subWidget : modelChildren)
    {
        Widget* child = dynamic_cast<Widget*>(subWidget);
        if (child)
        {
            addChild(child->clone());
        }
    }
}

void Widget::copySpecialProperties(Widget* model)
{

}

void Widget::copyProperties(Widget *widget)
{
    setEnabled(widget->isEnabled());
    setVisible(widget->isVisible());
    setBright(widget->isBright());
    setTouchEnabled(widget->isTouchEnabled());
    setLocalZOrder(widget->getLocalZOrder());
    setTag(widget->getTag());
    setName(widget->getName());
    setActionTag(widget->getActionTag());
    _ignoreSize = widget->_ignoreSize;
    this->setContentSize(widget->_contentSize);
    _customSize = widget->_customSize;
    _sizeType = widget->getSizeType();
    _sizePercent = widget->_sizePercent;
    _positionType = widget->_positionType;
    _positionPercent = widget->_positionPercent;
    setPosition(widget->getPosition());
    setAnchorPoint(widget->getAnchorPoint());
    setScaleX(widget->getScaleX());
    setScaleY(widget->getScaleY());
    setRotation(widget->getRotation());
    setRotationSkewX(widget->getRotationSkewX());
    setRotationSkewY(widget->getRotationSkewY());
    setFlippedX(widget->isFlippedX());
    setFlippedY(widget->isFlippedY());
    setColor(widget->getColor());
    setOpacity(widget->getOpacity());
    _touchEventCallback = widget->_touchEventCallback;
    _clickEventListener = widget->_clickEventListener;
    _focused = widget->_focused;
    _focusEnabled = widget->_focusEnabled;
    _propagateTouchEvents = widget->_propagateTouchEvents;
    
    copySpecialProperties(widget);

    Map<int, LayoutParameter*>& layoutParameterDic = widget->_layoutParameterDictionary;
    for (auto iter = layoutParameterDic.begin(); iter != layoutParameterDic.end(); ++iter)
    {
        setLayoutParameter(iter->second->clone());
    }
}
    
    void Widget::setFlippedX(bool flippedX)
    {
        
        float realScale = this->getScaleX();
        _flippedX = flippedX;
        this->setScaleX(realScale);
    }
    
    void Widget::setFlippedY(bool flippedY)
    {
        float realScale = this->getScaleY();
        _flippedY = flippedY;
        this->setScaleY(realScale);
    }
    
   
    
    void Widget::setScaleX(float scaleX)
    {
        if (_flippedX) {
            scaleX = scaleX * -1;
        }
        Node::setScaleX(scaleX);
    }
    
    void Widget::setScaleY(float scaleY)
    {
        if (_flippedY) {
            scaleY = scaleY * -1;
        }
        Node::setScaleY(scaleY);
    }
    
    void Widget::setScale(float scale)
    {
        this->setScaleX(scale);
        this->setScaleY(scale);
        this->setScaleZ(scale);
    }
    
    void Widget::setScale(float scaleX, float scaleY)
    {
        this->setScaleX(scaleX);
        this->setScaleY(scaleY);
    }
    
    float Widget::getScaleX()const
    {
        float originalScale = Node::getScaleX();
        if (_flippedX)
        {
            originalScale = originalScale * -1.0;
        }
        return originalScale;
    }
    
    float Widget::getScaleY()const
    {
        float originalScale = Node::getScaleY();
        if (_flippedY)
        {
            originalScale = originalScale * -1.0;
        }
        return originalScale;
    }
    
    float Widget::getScale()const
    {
        CCASSERT(this->getScaleX() == this->getScaleY(), "");
        return this->getScaleX();
    }


/*temp action*/
void Widget::setActionTag(int tag)
{
	_actionTag = tag;
}

int Widget::getActionTag()const
{
	return _actionTag;
}
    
void Widget::setFocused(bool focus)
{
    _focused = focus;
    
    //make sure there is only one focusedWidget
    if (focus) {
        _focusedWidget = this;
        if (_focusNavigationController) {
            _focusNavigationController->setFirstFocsuedWidget(this);
        }
    }
    
}

bool Widget::isFocused()const
{
    return _focused;
}

void Widget::setFocusEnabled(bool enable)
{
    _focusEnabled = enable;
}

bool Widget::isFocusEnabled()const
{
    return _focusEnabled;
}

Widget* Widget::findNextFocusedWidget(FocusDirection direction,  Widget* current)
{
    if (nullptr == onNextFocusedWidget || nullptr == onNextFocusedWidget(direction) ) {
        if (this->isFocused() || dynamic_cast<Layout*>(current))
        {
            Node* parent = this->getParent();
            
            Layout* layout = dynamic_cast<Layout*>(parent);
            if (nullptr == layout)
            {
                //the outer layout's default behaviour is : loop focus
                if (dynamic_cast<Layout*>(current))
                {
                    return current->findNextFocusedWidget(direction, current);
                }
                return current;
            }
            else
            {
                Widget *nextWidget = layout->findNextFocusedWidget(direction, current);
                return nextWidget;
            }
        }
        else
        {
            return current;
        }
    }
    else
    {
        Widget *getFocusWidget = onNextFocusedWidget(direction);
        this->dispatchFocusEvent(this, getFocusWidget);
        return getFocusWidget;
    }
}

void Widget::dispatchFocusEvent(cocos2d::ui::Widget *widgetLoseFocus, cocos2d::ui::Widget *widgetGetFocus)
{
    //if the widgetLoseFocus doesn't get focus, it will use the previous focused widget instead
    if (widgetLoseFocus && !widgetLoseFocus->isFocused())
    {
        widgetLoseFocus = _focusedWidget;
    }
    
    if (widgetGetFocus != widgetLoseFocus)
    {
        
        if (widgetGetFocus)
        {
            widgetGetFocus->onFocusChanged(widgetLoseFocus, widgetGetFocus);
        }
        
        if (widgetLoseFocus)
        {
            widgetLoseFocus->onFocusChanged(widgetLoseFocus, widgetGetFocus);
        }
        
        EventFocus event(widgetLoseFocus, widgetGetFocus);
        auto dispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
        dispatcher->dispatchEvent(&event);
    }
    
}

void Widget::requestFocus()
{
    if (this == _focusedWidget)
    {
        return;
    }
    
    this->dispatchFocusEvent(_focusedWidget, this);
}
    
void Widget::onFocusChange(Widget* widgetLostFocus, Widget* widgetGetFocus)
{
    //only change focus when there is indeed a get&lose happens
    if (widgetLostFocus)
    {
        widgetLostFocus->setFocused(false);
    }
    
    if (widgetGetFocus)
    {
        widgetGetFocus->setFocused(true);
    }
}

Widget* Widget::getCurrentFocusedWidget()const
{
    return _focusedWidget;
}

void Widget::enableDpadNavigation(bool enable)
{
    if (enable)
    {
        if (nullptr == _focusNavigationController)
        {
            _focusNavigationController = new (std::nothrow) FocusNavigationController;
            if (_focusedWidget) {
                _focusNavigationController->setFirstFocsuedWidget(_focusedWidget);
            }
        }
    }
    else
    {
        CC_SAFE_DELETE(_focusNavigationController);
    }
    _focusNavigationController->enableFocusNavigation(enable);
}


bool Widget::isUnifySizeEnabled()const
{
    return _unifySize;
}

void Widget::setUnifySizeEnabled(bool enable)
{
    _unifySize = enable;
}

void Widget::setLayoutComponentEnabled(bool enable)
{
	_usingLayoutComponent = enable;
}

bool Widget::isLayoutComponentEnabled()const
{
	return _usingLayoutComponent;
}


}
NS_CC_END
