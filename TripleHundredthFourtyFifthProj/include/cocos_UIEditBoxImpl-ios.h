/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 
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

#ifndef __UIEditBoxIMPLIOS_H__
#define __UIEditBoxIMPLIOS_H__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "cocos_UIEditBoxImpl-common.h"



@class UIEditBoxImplIOS_objc;
@class UIFont;

NS_CC_BEGIN

namespace ui {

class EditBox;

class EditBoxImplIOS : public EditBoxImplCommon
{
public:
    /**
     * @js NA
     */
    EditBoxImplIOS(EditBox* pEditText);
    /**
     * @js NA
     * @lua NA
     */
    virtual ~EditBoxImplIOS();
    
    virtual bool isEditing() override;
    virtual void createNativeControl(const Rect& frame) override;
    virtual void setNativeFont(const char* pFontName, int fontSize) override;
    virtual void setNativeFontColor(const Color4B& color) override;
    virtual void setNativePlaceholderFont(const char* pFontName, int fontSize) override;
    virtual void setNativePlaceholderFontColor(const Color4B& color) override;
    virtual void setNativeInputMode(EditBox::InputMode inputMode) override;
    virtual void setNativeInputFlag(EditBox::InputFlag inputFlag) override;
    virtual void setNativeReturnType(EditBox::KeyboardReturnType returnType)override;
    virtual void setNativeTextHorizontalAlignment(cocos2d::TextHAlignment alignment) override;
    virtual void setNativeText(const char* pText) override;
    virtual void setNativePlaceHolder(const char* pText) override;
    virtual void setNativeVisible(bool visible) override;
    virtual void updateNativeFrame(const Rect& rect) override;
    virtual const char* getNativeDefaultFontName() override;
    virtual void nativeOpenKeyboard() override;
    virtual void nativeCloseKeyboard() override;
    virtual void setNativeMaxLength(int maxLength) override;
    
    //need to remove siri text
    virtual const char* getText(void)override;

    virtual void doAnimationWhenKeyboardMove(float duration, float distance) override;
private:
    UIFont*         constructFont(const char* fontName, int fontSize);
    
    UIEditBoxImplIOS_objc* _systemControl;
};


}

NS_CC_END


#endif /* #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS) */

#endif /* __UIEditBoxIMPLIOS_H__ */

