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

#include "cocos_UICheckBox.h"
#include "cocos_CCSprite.h"

NS_CC_BEGIN

namespace ui {
    
static const int BACKGROUNDBOX_RENDERER_Z = (-1);
static const int BACKGROUNDSELECTEDBOX_RENDERER_Z = (-1);
static const int FRONTCROSS_RENDERER_Z = (-1);
static const int BACKGROUNDBOXDISABLED_RENDERER_Z = (-1);
static const int FRONTCROSSDISABLED_RENDERER_Z = (-1);
    
IMPLEMENT_CLASS_GUI_INFO(CheckBox)

CheckBox::CheckBox():
_backGroundBoxRenderer(nullptr),
_backGroundSelectedBoxRenderer(nullptr),
_frontCrossRenderer(nullptr),
_backGroundBoxDisabledRenderer(nullptr),
_frontCrossDisabledRenderer(nullptr),
_isSelected(true),
_backGroundTexType(TextureResType::LOCAL),
_backGroundSelectedTexType(TextureResType::LOCAL),
_frontCrossTexType(TextureResType::LOCAL),
_backGroundDisabledTexType(TextureResType::LOCAL),
_frontCrossDisabledTexType(TextureResType::LOCAL),
_backGroundFileName(""),
_backGroundSelectedFileName(""),
_frontCrossFileName(""),
_backGroundDisabledFileName(""),
_frontCrossDisabledFileName(""),
_backGroundBoxRendererAdaptDirty(true),
_backGroundSelectedBoxRendererAdaptDirty(true),
_frontCrossRendererAdaptDirty(true),
_backGroundBoxDisabledRendererAdaptDirty(true),
_frontCrossDisabledRendererAdaptDirty(true)
{
    setTouchEnabled(true);
}

CheckBox::~CheckBox()
{
}

CheckBox* CheckBox::create()
{
    CheckBox* widget = new (std::nothrow) CheckBox();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return nullptr;
}
    
CheckBox* CheckBox::create(const std::string& backGround,
                           const std::string& backGroundSeleted,
                           const std::string& cross,
                           const std::string& backGroundDisabled,
                           const std::string& frontCrossDisabled,
                           TextureResType texType)
{
    CheckBox *pWidget = new (std::nothrow) CheckBox;
    if (pWidget && pWidget->init(backGround,
                                 backGroundSeleted,
                                 cross,
                                 backGroundDisabled,
                                 frontCrossDisabled,
                                 texType))
    {
        pWidget->autorelease();
        return pWidget;
    }
    CC_SAFE_DELETE(pWidget);
    return nullptr;
}
    
bool CheckBox::init(const std::string& backGround,
                    const std::string& backGroundSeleted,
                    const std::string& cross,
                    const std::string& backGroundDisabled,
                    const std::string& frontCrossDisabled,
                    TextureResType texType)
{
    bool ret = true;
    do {
        if (!Widget::init()) {
            ret = false;
            break;
        }
        
        setSelected(false);
        loadTextures(backGround, backGroundSeleted, cross, backGroundDisabled, frontCrossDisabled,texType);
    } while (0);
    return ret;
}

bool CheckBox::init()
{
    if (Widget::init())
    {
        setSelected(false);
        return true;
    }
    return false;
}

void CheckBox::initRenderer()
{
    _backGroundBoxRenderer = Sprite::create();
    _backGroundSelectedBoxRenderer = Sprite::create();
    _frontCrossRenderer = Sprite::create();
    _backGroundBoxDisabledRenderer = Sprite::create();
    _frontCrossDisabledRenderer = Sprite::create();
        
    addProtectedChild(_backGroundBoxRenderer, BACKGROUNDBOX_RENDERER_Z, -1);
    addProtectedChild(_backGroundSelectedBoxRenderer, BACKGROUNDSELECTEDBOX_RENDERER_Z, -1);
    addProtectedChild(_frontCrossRenderer, FRONTCROSS_RENDERER_Z, -1);
    addProtectedChild(_backGroundBoxDisabledRenderer, BACKGROUNDBOXDISABLED_RENDERER_Z, -1);
    addProtectedChild(_frontCrossDisabledRenderer, FRONTCROSSDISABLED_RENDERER_Z, -1);
}

void CheckBox::loadTextures(const std::string& backGround,
                            const std::string& backGroundSelected,
                            const std::string& cross,
                            const std::string& backGroundDisabled,
                            const std::string& frontCrossDisabled,
                            TextureResType texType)
{
    loadTextureBackGround(backGround,texType);
    loadTextureBackGroundSelected(backGroundSelected,texType);
    loadTextureFrontCross(cross,texType);
    loadTextureBackGroundDisabled(backGroundDisabled,texType);
    loadTextureFrontCrossDisabled(frontCrossDisabled,texType);
}

void CheckBox::loadTextureBackGround(const std::string& backGround,TextureResType texType)
{
    if (backGround.empty() || (_backGroundFileName == backGround && _backGroundTexType == texType))
    {
        return;
    }
    _backGroundFileName = backGround;
    _backGroundTexType = texType;
    switch (_backGroundTexType)
    {
        case TextureResType::LOCAL:
            _backGroundBoxRenderer->setTexture(backGround);
            break;
        case TextureResType::PLIST:
            _backGroundBoxRenderer->setSpriteFrame(backGround);
            break;
        default:
            break;
    }
   
    this->updateChildrenDisplayedRGBA();

    updateContentSizeWithTextureSize(_backGroundBoxRenderer->getContentSize());
    _backGroundBoxRendererAdaptDirty = true;
}

void CheckBox::loadTextureBackGroundSelected(const std::string& backGroundSelected,TextureResType texType)
{
    if (backGroundSelected.empty() || (_backGroundSelectedFileName == backGroundSelected && _backGroundSelectedTexType == texType))
    {
        return;
    }
    _backGroundSelectedFileName = backGroundSelected;
    _backGroundSelectedTexType = texType;
    switch (_backGroundSelectedTexType)
    {
        case TextureResType::LOCAL:
            _backGroundSelectedBoxRenderer->setTexture(backGroundSelected);
            break;
        case TextureResType::PLIST:
            _backGroundSelectedBoxRenderer->setSpriteFrame(backGroundSelected);
            break;
        default:
            break;
    }
  
    this->updateChildrenDisplayedRGBA();

    _backGroundSelectedBoxRendererAdaptDirty = true;
}

void CheckBox::loadTextureFrontCross(const std::string& cross,TextureResType texType)
{
    if (cross.empty() || (_frontCrossFileName == cross && _frontCrossTexType == texType))
    {
        return;
    }
    _frontCrossFileName = cross;
    _frontCrossTexType = texType;
    switch (_frontCrossTexType)
    {
        case TextureResType::LOCAL:
            _frontCrossRenderer->setTexture(cross);
            break;
        case TextureResType::PLIST:
            _frontCrossRenderer->setSpriteFrame(cross);
            break;
        default:
            break;
    }
   
    this->updateChildrenDisplayedRGBA();

    _frontCrossRendererAdaptDirty = true;
}

void CheckBox::loadTextureBackGroundDisabled(const std::string& backGroundDisabled,TextureResType texType)
{
    if (backGroundDisabled.empty() || (_backGroundDisabledFileName == backGroundDisabled && _backGroundDisabledTexType == texType))
    {
        return;
    }
    _backGroundDisabledFileName = backGroundDisabled;
    _backGroundDisabledTexType = texType;
    switch (_backGroundDisabledTexType)
    {
        case TextureResType::LOCAL:
            _backGroundBoxDisabledRenderer->setTexture(backGroundDisabled);
            break;
        case TextureResType::PLIST:
            _backGroundBoxDisabledRenderer->setSpriteFrame(backGroundDisabled);
            break;
        default:
            break;
    }
   
    this->updateChildrenDisplayedRGBA();

    _backGroundBoxDisabledRendererAdaptDirty = true;
}

void CheckBox::loadTextureFrontCrossDisabled(const std::string& frontCrossDisabled,TextureResType texType)
{
    if (frontCrossDisabled.empty() || (_frontCrossDisabledFileName == frontCrossDisabled && _frontCrossDisabledTexType == texType))
    {
        return;
    }
    _frontCrossDisabledFileName = frontCrossDisabled;
    _frontCrossDisabledTexType = texType;
    switch (_frontCrossDisabledTexType)
    {
        case TextureResType::LOCAL:
            _frontCrossDisabledRenderer->setTexture(frontCrossDisabled);
            break;
        case TextureResType::PLIST:
            _frontCrossDisabledRenderer->setSpriteFrame(frontCrossDisabled);
            break;
        default:
            break;
    }
  
    this->updateChildrenDisplayedRGBA();

    _frontCrossDisabledRendererAdaptDirty = true;
}


void CheckBox::releaseUpEvent()
{
    Widget::releaseUpEvent();
    
    if (_isSelected){
        setSelected(false);
        unSelectedEvent();
    }
    else
    {
        setSelected(true);
        selectedEvent();
    }
}


void CheckBox::onPressStateChangedToNormal()
{
    _backGroundBoxRenderer->setVisible(true);
    _backGroundSelectedBoxRenderer->setVisible(false);
    _backGroundBoxDisabledRenderer->setVisible(false);
    _frontCrossDisabledRenderer->setVisible(false);
    if (_isSelected)
    {
        _frontCrossRenderer->setVisible(true);
    }
}

void CheckBox::onPressStateChangedToPressed()
{
    _backGroundBoxRenderer->setVisible(false);
    _backGroundSelectedBoxRenderer->setVisible(true);
    _backGroundBoxDisabledRenderer->setVisible(false);
    _frontCrossDisabledRenderer->setVisible(false);
}

void CheckBox::onPressStateChangedToDisabled()
{
    _backGroundBoxRenderer->setVisible(false);
    _backGroundSelectedBoxRenderer->setVisible(false);
    _backGroundBoxDisabledRenderer->setVisible(true);
    _frontCrossRenderer->setVisible(false);
    if (_isSelected)
    {
        _frontCrossDisabledRenderer->setVisible(true);
    }
}

void CheckBox::setSelected(bool selected)
{
    if (selected == _isSelected)
    {
        return;
    }
    _isSelected = selected;
    _frontCrossRenderer->setVisible(_isSelected);
}
    
bool CheckBox::isSelected()const
{
    return _isSelected;
}

void CheckBox::selectedEvent()
{
    this->retain();
    if (_checkBoxEventCallback)
    {
        _checkBoxEventCallback(this, EventType::SELECTED);
    }
    if (_ccEventCallback)
    {
        _ccEventCallback(this, static_cast<int>(EventType::SELECTED));
    }
    this->release();
}

void CheckBox::unSelectedEvent()
{
    this->retain();
    if (_checkBoxEventCallback) {
        _checkBoxEventCallback(this, EventType::UNSELECTED);
    }
    if (_ccEventCallback)
    {
        _ccEventCallback(this, static_cast<int>(EventType::UNSELECTED));
    }
    this->release();
}

void CheckBox::addEventListener(const ccCheckBoxCallback& callback)
{
    _checkBoxEventCallback = callback;
}
    
void CheckBox::onSizeChanged()
{
    Widget::onSizeChanged();
    _backGroundBoxRendererAdaptDirty = true;
    _backGroundSelectedBoxRendererAdaptDirty = true;
    _frontCrossRendererAdaptDirty = true;
    _backGroundBoxDisabledRendererAdaptDirty = true;
    _frontCrossDisabledRendererAdaptDirty = true;
}
    
void CheckBox::adaptRenderers()
{
    if (_backGroundBoxRendererAdaptDirty)
    {
        backGroundTextureScaleChangedWithSize();
        _backGroundBoxRendererAdaptDirty = false;
    }
    if (_backGroundSelectedBoxRendererAdaptDirty)
    {
        backGroundSelectedTextureScaleChangedWithSize();
        _backGroundSelectedBoxRendererAdaptDirty = false;
    }
    if (_frontCrossRendererAdaptDirty)
    {
        frontCrossTextureScaleChangedWithSize();
        _frontCrossRendererAdaptDirty = false;
    }
    if (_backGroundBoxDisabledRendererAdaptDirty)
    {
        backGroundDisabledTextureScaleChangedWithSize();
        _backGroundBoxDisabledRendererAdaptDirty = false;
    }
    if (_frontCrossDisabledRendererAdaptDirty)
    {
        frontCrossDisabledTextureScaleChangedWithSize();
        _frontCrossDisabledRendererAdaptDirty = false;
    }
}

Size CheckBox::getVirtualRendererSize() const
{
    return _backGroundBoxRenderer->getContentSize();
}

Node* CheckBox::getVirtualRenderer()
{
    return _backGroundBoxRenderer;
}

void CheckBox::backGroundTextureScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        _backGroundBoxRenderer->setScale(1.0f);
    }
    else
    {
        Size textureSize = _backGroundBoxRenderer->getContentSize();
        if (textureSize.width <= 0.0f || textureSize.height <= 0.0f)
        {
            _backGroundBoxRenderer->setScale(1.0f);
            return;
        }
        float scaleX = _contentSize.width / textureSize.width;
        float scaleY = _contentSize.height / textureSize.height;
        _backGroundBoxRenderer->setScaleX(scaleX);
        _backGroundBoxRenderer->setScaleY(scaleY);
    }
    _backGroundBoxRenderer->setPosition(_contentSize.width / 2, _contentSize.height / 2);
}

void CheckBox::backGroundSelectedTextureScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        _backGroundSelectedBoxRenderer->setScale(1.0f);
    }
    else
    {
        Size textureSize = _backGroundSelectedBoxRenderer->getContentSize();
        if (textureSize.width <= 0.0f || textureSize.height <= 0.0f)
        {
            _backGroundSelectedBoxRenderer->setScale(1.0f);
            return;
        }
        float scaleX = _contentSize.width / textureSize.width;
        float scaleY = _contentSize.height / textureSize.height;
        _backGroundSelectedBoxRenderer->setScaleX(scaleX);
        _backGroundSelectedBoxRenderer->setScaleY(scaleY);
    }
    _backGroundSelectedBoxRenderer->setPosition(_contentSize.width / 2, _contentSize.height / 2);
}

void CheckBox::frontCrossTextureScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        _frontCrossRenderer->setScale(1.0f);
    }
    else
    {
        Size textureSize = _frontCrossRenderer->getContentSize();
        if (textureSize.width <= 0.0f || textureSize.height <= 0.0f)
        {
            _frontCrossRenderer->setScale(1.0f);
            return;
        }
        float scaleX = _contentSize.width / textureSize.width;
        float scaleY = _contentSize.height / textureSize.height;
        _frontCrossRenderer->setScaleX(scaleX);
        _frontCrossRenderer->setScaleY(scaleY);
    }
    _frontCrossRenderer->setPosition(_contentSize.width / 2, _contentSize.height / 2);
}

void CheckBox::backGroundDisabledTextureScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        _backGroundBoxDisabledRenderer->setScale(1.0f);
    }
    else
    {
        Size textureSize = _backGroundBoxDisabledRenderer->getContentSize();
        if (textureSize.width <= 0.0f || textureSize.height <= 0.0f)
        {
            _backGroundBoxDisabledRenderer->setScale(1.0f);
            return;
        }
        float scaleX = _contentSize.width / textureSize.width;
        float scaleY = _contentSize.height / textureSize.height;
        _backGroundBoxDisabledRenderer->setScaleX(scaleX);
        _backGroundBoxDisabledRenderer->setScaleY(scaleY);
    }
    _backGroundBoxDisabledRenderer->setPosition(_contentSize.width / 2, _contentSize.height / 2);
}

void CheckBox::frontCrossDisabledTextureScaleChangedWithSize()
{
    if (_ignoreSize)
    {
        _frontCrossDisabledRenderer->setScale(1.0f);
    }
    else
    {
        Size textureSize = _frontCrossDisabledRenderer->getContentSize();
        if (textureSize.width <= 0.0f || textureSize.height <= 0.0f)
        {
            _frontCrossDisabledRenderer->setScale(1.0f);
            return;
        }
        float scaleX = _contentSize.width / textureSize.width;
        float scaleY = _contentSize.height / textureSize.height;
        _frontCrossDisabledRenderer->setScaleX(scaleX);
        _frontCrossDisabledRenderer->setScaleY(scaleY);
    }
    _frontCrossDisabledRenderer->setPosition(_contentSize.width / 2, _contentSize.height / 2);
}

std::string CheckBox::getDescription() const
{
    return "CheckBox";
}

Widget* CheckBox::createCloneInstance()
{
    return CheckBox::create();
}

void CheckBox::copySpecialProperties(Widget *widget)
{
    CheckBox* checkBox = dynamic_cast<CheckBox*>(widget);
    if (checkBox)
    {
        loadTextureBackGround(checkBox->_backGroundFileName, checkBox->_backGroundTexType);
        loadTextureBackGroundSelected(checkBox->_backGroundSelectedFileName, checkBox->_backGroundSelectedTexType);
        loadTextureFrontCross(checkBox->_frontCrossFileName, checkBox->_frontCrossTexType);
        loadTextureBackGroundDisabled(checkBox->_backGroundDisabledFileName, checkBox->_backGroundDisabledTexType);
        loadTextureFrontCrossDisabled(checkBox->_frontCrossDisabledFileName, checkBox->_frontCrossDisabledTexType);
        setSelected(checkBox->_isSelected);
        _checkBoxEventCallback = checkBox->_checkBoxEventCallback;
        _ccEventCallback = checkBox->_ccEventCallback;
    }
}

}

NS_CC_END
