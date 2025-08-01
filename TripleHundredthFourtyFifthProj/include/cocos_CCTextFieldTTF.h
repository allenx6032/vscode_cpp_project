/****************************************************************************
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

#ifndef __CC_TEXT_FIELD_H__
#define __CC_TEXT_FIELD_H__

#include "cocos_CCLabel.h"
#include "cocos_CCIMEDelegate.h"

NS_CC_BEGIN

class TextFieldTTF;

/**
 * @addtogroup input
 * @{
 */

class CC_DLL TextFieldDelegate
{
public:
    virtual ~TextFieldDelegate() {}

    /**
    @brief    If the sender doesn't want to attach to the IME, return true;
    */
    virtual bool onTextFieldAttachWithIME(TextFieldTTF * sender)
    {
        CC_UNUSED_PARAM(sender);
        return false;
    }

    /**
    @brief    If the sender doesn't want to detach from the IME, return true;
    */
    virtual bool onTextFieldDetachWithIME(TextFieldTTF * sender)
    {
        CC_UNUSED_PARAM(sender);
        return false;
    }

    /**
    @brief    If the sender doesn't want to insert the text, return true;
    */
    virtual bool onTextFieldInsertText(TextFieldTTF * sender, const char * text, size_t nLen)
    {
        CC_UNUSED_PARAM(sender);
        CC_UNUSED_PARAM(text);
        CC_UNUSED_PARAM(nLen);
        return false;
    }

    /**
    @brief    If the sender doesn't want to delete the delText, return true;
    */
    virtual bool onTextFieldDeleteBackward(TextFieldTTF * sender, const char * delText, size_t nLen)
    {
        CC_UNUSED_PARAM(sender);
        CC_UNUSED_PARAM(delText);
        CC_UNUSED_PARAM(nLen);
        return false;
    }

    /**
    @brief    If the sender doesn't want to draw, return true.
    */
    virtual bool onVisit(TextFieldTTF * sender,Renderer *renderer, const Mat4 &transform, uint32_t flags)
    {
        CC_UNUSED_PARAM(sender);
        return false;
    }
};

/**
@brief    A simple text input field with TTF font.
*/
class CC_DLL TextFieldTTF : public Label, public IMEDelegate
{
public:
    /**
     * @js ctor
     */
    TextFieldTTF();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~TextFieldTTF();

    //char * description();

    /** creates a TextFieldTTF from a fontname, alignment, dimension and font size */
    static TextFieldTTF * textFieldWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);
    /** creates a TextFieldTTF from a fontname and font size */
    static TextFieldTTF * textFieldWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);
    /** initializes the TextFieldTTF with a font name, alignment, dimension and font size */
    bool initWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);
    /** initializes the TextFieldTTF with a font name and font size */
    bool initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);

    /**
    @brief    Open keyboard and receive input text.
    */
    virtual bool attachWithIME() override;

    /**
    @brief    End text input and close keyboard.
    */
    virtual bool detachWithIME() override;

    //////////////////////////////////////////////////////////////////////////
    // properties
    //////////////////////////////////////////////////////////////////////////
    /**
     * @js NA
     * @lua NA
     */
    inline TextFieldDelegate* getDelegate() const { return _delegate; };
    /**
     * @js NA
     * @lua NA
     */
    inline void setDelegate(TextFieldDelegate* delegate) { _delegate = delegate; };

    inline int getCharCount() const { return _charCount; };
    
    virtual const Color4B& getColorSpaceHolder();

    virtual void setColorSpaceHolder(const Color3B& color);
    virtual void setColorSpaceHolder(const Color4B& color);

    virtual void setTextColor(const Color4B& textColor) override;

    // input text property
    virtual void setString(const std::string& text) override;
    virtual const std::string& getString() const override;

    // place holder text property
    // place holder text displayed when there is no text in the text field.
    virtual void setPlaceHolder(const std::string& text);
    virtual const std::string& getPlaceHolder() const;

    virtual void setSecureTextEntry(bool value);
    virtual bool isSecureTextEntry();

    virtual void visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags) override;

protected:
    //////////////////////////////////////////////////////////////////////////
    // IMEDelegate interface
    //////////////////////////////////////////////////////////////////////////

    virtual bool canAttachWithIME() override;
    virtual bool canDetachWithIME() override;
    virtual void insertText(const char * text, size_t len) override;
    virtual void deleteBackward() override;
    virtual const std::string& getContentText() override;

    TextFieldDelegate * _delegate;
    int _charCount;

    std::string _inputText;

    std::string _placeHolder;
    Color4B _colorSpaceHolder;
    Color4B _colorText;

    bool _secureTextEntry;

private:
    class LengthStack;
    LengthStack * _lens;
};

// end of input group
/// @}

NS_CC_END

#endif    // __CC_TEXT_FIELD_H__
