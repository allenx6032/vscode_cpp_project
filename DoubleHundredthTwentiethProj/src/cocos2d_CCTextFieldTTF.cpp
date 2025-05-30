/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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

#include "cocos2d_CCTextFieldTTF.h"

#include "cocos2d_CCSprite.h"
#include "cocos2d_CCConsole.h"
#include "cocos2d_CCDirector.h"
#include "cocos2d_ccUTF8.h"
#include "cocos2d_CCFileUtils.h"

NS_CC_BEGIN

#define CURSOR_TIME_SHOW_HIDE 0.5f
#define CURSOR_DEFAULT_CHAR '|'
#define PASSWORD_STYLE_TEXT_DEFAULT "\xe2\x80\xa2"

static std::size_t _calcCharCount(const char* textPtr)
{
    int charCount = 0;

    while (char nextChar = *textPtr)
    {
        if (0x80 != (0xC0 & nextChar))
        {
            charCount++;
        }

        textPtr++;
    }

    return charCount;
}

static std::size_t _cursorPositionToCharPosition(const char* textPtr, int cursorPos)
{
    int byteCount = 0;
    int charCount = 0;

    while (char nextChar = *textPtr)
    {
        if (charCount >= cursorPos)
        {
            break;
        }

        if (0x80 != (0xC0 & nextChar))
        {
            charCount++;
        }

        textPtr++;
        byteCount++;
    }

    return byteCount;
}

static std::size_t _charPositionToCursorPosition(const char* textPtr, int bytePos)
{
    int byteCount = 0;
    int charCount = 0;

    while (char nextChar = *textPtr)
    {
        if (byteCount >= bytePos)
        {
            break;
        }

        if (0x80 != (0xC0 & nextChar))
        {
            charCount++;
        }

        textPtr++;
        byteCount++;
    }

    return charCount;
}

bool TextFieldDelegate::onTextFieldAttachWithIME(TextFieldTTF* /*sender*/)
{
    return false;
}

bool TextFieldDelegate::onTextFieldDetachWithIME(TextFieldTTF* /*sender*/)
{
    return false;
}

bool TextFieldDelegate::onTextFieldInsertText(TextFieldTTF* /*sender*/, const char* /*text*/, size_t /*nLen*/)
{
    return false;
}

bool TextFieldDelegate::onTextFieldDeleteBackward(TextFieldTTF* /*sender*/, const char* /*delText*/, size_t /*nLen*/)
{
    return false;
}

bool TextFieldDelegate::onVisit(TextFieldTTF* /*sender*/, Renderer* /*renderer*/, const Mat4& /*transform*/, uint32_t /*flags*/)
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// constructor and destructor
//////////////////////////////////////////////////////////////////////////

TextFieldTTF::TextFieldTTF()
: _delegate(0)
, _charCount(0)
, _inputText("")
, _placeHolder("")   // prevent Label initWithString assertion
, _colorText(Color4B::WHITE)
, _secureTextEntry(false)
, _passwordStyleText(PASSWORD_STYLE_TEXT_DEFAULT)
, _cursorEnabled(false)
, _cursorPosition(0)
, _cursorChar(CURSOR_DEFAULT_CHAR)
, _cursorShowingTime(0.0f)
, _isAttachWithIME(false)
{
    _colorSpaceHolder.r = _colorSpaceHolder.g = _colorSpaceHolder.b = 127;
    _colorSpaceHolder.a = 255;
}

TextFieldTTF::~TextFieldTTF()
{
}

//////////////////////////////////////////////////////////////////////////
// static constructor
//////////////////////////////////////////////////////////////////////////

TextFieldTTF * TextFieldTTF::textFieldWithPlaceHolder(const std::string& placeholder, const CSize& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize)
{
    TextFieldTTF *ret = new (std::nothrow) TextFieldTTF();
    if(ret && ret->initWithPlaceHolder("", dimensions, alignment, fontName, fontSize))
    {
        ret->autorelease();
        if (placeholder.size()>0)
        {
            ret->setPlaceHolder(placeholder);
        }
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

TextFieldTTF * TextFieldTTF::textFieldWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize)
{
    TextFieldTTF *ret = new (std::nothrow) TextFieldTTF();
    if(ret && ret->initWithPlaceHolder("", fontName, fontSize))
    {
        ret->autorelease();
        if (placeholder.size()>0)
        {
            ret->setPlaceHolder(placeholder);
        }
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// initialize
//////////////////////////////////////////////////////////////////////////

bool TextFieldTTF::initWithPlaceHolder(const std::string& placeholder, const CSize& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize)
{
    setDimensions(dimensions.width, dimensions.height);
    setAlignment(alignment, TextVAlignment::CENTER);

    return initWithPlaceHolder(placeholder, fontName, fontSize);
}
bool TextFieldTTF::initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize)
{
    _placeHolder = placeholder;

    do 
    {
        // If fontName is ttf file and it corrected, use TTFConfig
        if (FileUtils::getInstance()->isFileExist(fontName))
        {
            TTFConfig ttfConfig(fontName, fontSize, CGlyphCollection::DYNAMIC);
            if (setTTFConfig(ttfConfig))
            {
                break;
            }
        }

        setSystemFontName(fontName);
        setSystemFontSize(fontSize);

    } while (false);
    

    Label::setTextColor(_colorSpaceHolder);
    Label::setString(_placeHolder);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
    // On desktop default enable cursor
    if (_currentLabelType == LabelType::TTF)
    {
        setCursorEnabled(true);
    }
#endif

    return true;
}

//////////////////////////////////////////////////////////////////////////
// IMEDelegate
//////////////////////////////////////////////////////////////////////////

bool TextFieldTTF::attachWithIME()
{
    bool ret = IMEDelegate::attachWithIME();
    if (ret)
    {
        // open keyboard
        auto pGlView = Director::getInstance()->getOpenGLView();
        if (pGlView)
        {
            pGlView->setIMEKeyboardState(true);
        }
    }
    return ret;
}

bool TextFieldTTF::detachWithIME()
{
    bool ret = IMEDelegate::detachWithIME();
    if (ret)
    {
        // close keyboard
        auto glView = Director::getInstance()->getOpenGLView();
        if (glView)
        {
            glView->setIMEKeyboardState(false);
        }
    }
    return ret;
}

void TextFieldTTF::didAttachWithIME()
{
    setAttachWithIME(true);
}

void TextFieldTTF::didDetachWithIME()
{
    setAttachWithIME(false);
}

bool TextFieldTTF::canAttachWithIME()
{
    return (_delegate) ? (! _delegate->onTextFieldAttachWithIME(this)) : true;
}

bool TextFieldTTF::canDetachWithIME()
{
    return (_delegate) ? (! _delegate->onTextFieldDetachWithIME(this)) : true;
}

void TextFieldTTF::insertText(const char * text, size_t len)
{
    std::string insert(text, len);

    // insert \n means input end
    int pos = static_cast<int>(insert.find((char)StringUtils::AsciiCharacters::NewLine));
    if ((int)insert.npos != pos)
    {
        // len = pos;
        // insert.erase(pos);
    }

    if (len > 0)
    {
        if (_delegate && _delegate->onTextFieldInsertText(this, insert.c_str(), len))
        {
            // delegate doesn't want to insert text
            return;
        }

        std::size_t countInsertChar = _calcCharCount(insert.c_str());
        _charCount += countInsertChar;

        if (_cursorEnabled)
        {
            StringUtils::StringUTF8 stringUTF8;

            stringUTF8.replace(_inputText);
            stringUTF8.insert(_cursorPosition, insert);

            setCursorPosition(_cursorPosition + countInsertChar);

            setString(stringUTF8.getAsCharSequence());
        }
        else
        {
            std::string sText(_inputText);
            sText.append(insert);
            setString(sText);
        }
    }

    if ((int)insert.npos == pos) {
        return;
    }

    // '\n' inserted, let delegate process first
    if (_delegate && _delegate->onTextFieldInsertText(this, "\n", 1))
    {
        return;
    }

    // if delegate hasn't processed, detach from IME by default
    //detachWithIME();
}

void TextFieldTTF::deleteBackward()
{
    size_t len = _inputText.length();
    if (! len)
    {
        // there is no string
        return;
    }

    // get the delete byte number
    size_t deleteLen = 1;    // default, erase 1 byte

    while(0x80 == (0xC0 & _inputText.at(len - deleteLen)))
    {
        ++deleteLen;
    }

    if (_delegate && _delegate->onTextFieldDeleteBackward(this, _inputText.c_str() + len - deleteLen, static_cast<int>(deleteLen)))
    {
        // delegate doesn't want to delete backwards
        return;
    }

    // if all text deleted, show placeholder string
    if (len <= deleteLen)
    {
        _inputText = "";
        _charCount = 0;
        setCursorPosition(0);
        setString(_inputText);
        return;
    }

    // set new input text
    if (_cursorEnabled)
    {
        if (_cursorPosition)
        {
            setCursorPosition(_cursorPosition - 1);

            StringUtils::StringUTF8 stringUTF8;

            stringUTF8.replace(_inputText);
            stringUTF8.deleteChar(_cursorPosition);

            _charCount = stringUTF8.length();
            setString(stringUTF8.getAsCharSequence());
        }
    }
    else
    {
        std::string text(_inputText.c_str(), len - deleteLen);
        setString(text);
    }
}

const std::string& TextFieldTTF::getContentText()
{
    return _inputText;
}

void TextFieldTTF::setCursorPosition(std::size_t cursorPosition)
{
    if (_cursorEnabled && cursorPosition <= (std::size_t)_charCount)
    {
        _cursorPosition = cursorPosition;
        _cursorShowingTime = CURSOR_TIME_SHOW_HIDE * 2.0f;
    }
}

void TextFieldTTF::setCursorFromPoint(const Vec2 &point, const Camera* camera)
{
    if (_cursorEnabled)
    {
        // Reset Label, no cursor
        bool oldIsAttachWithIME = _isAttachWithIME;
        _isAttachWithIME = false;
        updateCursorDisplayText();

        CRect rect;
        rect.size = getContentSize();
        if (isScreenPointInRect(point, camera, getWorldToNodeTransform(), rect, nullptr))
        {
            int latterPosition = 0;
            for (; latterPosition < _lengthOfString; ++latterPosition)
            {
                if (_lettersInfo[latterPosition].valid)
                {
                    auto sprite = getLetter(latterPosition);
                        rect.size = sprite->getContentSize();
                        if (isScreenPointInRect(point, camera, sprite->getWorldToNodeTransform(), rect, nullptr))
                        {
                            setCursorPosition(latterPosition);
                            break;
                        }
                    }
                }
            if (latterPosition == _lengthOfString)
            {
                setCursorPosition(latterPosition);
            }
        }

        // Set cursor
        _isAttachWithIME = oldIsAttachWithIME;
        updateCursorDisplayText();
    }
}

void TextFieldTTF::setAttachWithIME(bool isAttachWithIME)
{
    if (isAttachWithIME != _isAttachWithIME)
    {
        _isAttachWithIME = isAttachWithIME;

        if (_isAttachWithIME)
        {
            setCursorPosition(_charCount);
        }
        updateCursorDisplayText();
    }
}

void TextFieldTTF::setTextColor(const Color4B &color)
{
    _colorText = color;
    if (!_inputText.empty())
    {
        Label::setTextColor(_colorText);
    }
}

void TextFieldTTF::visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if (_delegate && _delegate->onVisit(this,renderer,parentTransform,parentFlags))
    {
        return;
    }
    Label::visit(renderer,parentTransform,parentFlags);
}

void TextFieldTTF::update(float delta)
{
    if (_cursorEnabled && _isAttachWithIME)
    {
        _cursorShowingTime -= delta;
        if (_cursorShowingTime < -CURSOR_TIME_SHOW_HIDE)
        {
            _cursorShowingTime = CURSOR_TIME_SHOW_HIDE;
        }
        // before cursor inserted '\b', need next letter
        auto sprite = getLetter((int)_cursorPosition + 1, true);

        if (sprite)
        {
            if (_cursorShowingTime >= 0.0f)
            {
                sprite->setOpacity(255);
            }
            else
            {
                sprite->setOpacity(0);
            }
            sprite->setDirty(true);
        }
    }
}

const Color4B& TextFieldTTF::getColorSpaceHolder()
{
    return _colorSpaceHolder;
}

void TextFieldTTF::setColorSpaceHolder(const Color3B& color)
{
    _colorSpaceHolder.r = color.r;
    _colorSpaceHolder.g = color.g;
    _colorSpaceHolder.b = color.b;
    _colorSpaceHolder.a = 255;
    if (_inputText.empty())
    {
        Label::setTextColor(_colorSpaceHolder);
}
}

void TextFieldTTF::setColorSpaceHolder(const Color4B& color)
{
    _colorSpaceHolder = color;
    if (_inputText.empty())
    {
        Label::setTextColor(_colorSpaceHolder);
    }
}

//////////////////////////////////////////////////////////////////////////
// properties
//////////////////////////////////////////////////////////////////////////

// input text property
void TextFieldTTF::setString(const std::string &text)
{
    std::string displayText;

    std::size_t charCount = 0;

    if (!text.empty())
    {
        _inputText = text;
        displayText = _inputText;
        charCount = _calcCharCount(_inputText.c_str());

        if (_secureTextEntry)
        {
            displayText = "";
            size_t length = charCount;

            while (length)
            {
                displayText.append(_passwordStyleText);
                length--;
            }
        }
    }
    else
    {
        _inputText = "";
    }

    if (_cursorEnabled && charCount != _charCount)
    {
        _cursorPosition = charCount;
    }

    if (_cursorEnabled)
    {
        // Need for recreate all letters in Label
		Label::removeLetters();
    }

    // if there is no input text, display placeholder instead
    if (_inputText.empty() && (!_cursorEnabled || !_isAttachWithIME))
    {
        Label::setTextColor(_colorSpaceHolder);
        Label::setString(_placeHolder);
    }
    else
    {
        makeStringSupportCursor(displayText);

        Label::setTextColor(_colorText);
        Label::setString(displayText);
    }
    _charCount = charCount;
}



void TextFieldTTF::appendString(const std::string& text)
{
    insertText(text.c_str(), text.length());
}

void TextFieldTTF::makeStringSupportCursor(std::string& displayText)
{
    if (_cursorEnabled && _isAttachWithIME)
    {
        if (displayText.empty())
        {
            // \b - Next char not change x position
            displayText.push_back((char)StringUtils::AsciiCharacters::NextCharNoChangeX);
            displayText.push_back(_cursorChar);
        }
        else
        {
            StringUtils::StringUTF8 stringUTF8;
            std::string cursorChar;

            stringUTF8.replace(displayText);

            if (_cursorPosition > stringUTF8.length())
            {
                _cursorPosition = stringUTF8.length();
            }

            // \b - Next char not change x position
            cursorChar.push_back((char)StringUtils::AsciiCharacters::NextCharNoChangeX);
            cursorChar.push_back(_cursorChar);
            stringUTF8.insert(_cursorPosition, cursorChar);

            displayText = stringUTF8.getAsCharSequence();
        }
    }
}

void TextFieldTTF::updateCursorDisplayText()
{
    // Update Label content
    setString(_inputText);
}

void TextFieldTTF::setCursorChar(char cursor)
{
    if (_cursorChar != cursor)
    {
        _cursorChar = cursor;
        updateCursorDisplayText();
    }
}

void TextFieldTTF::controlKey(InputEvents::KeyCode keyCode)
{
    if (_cursorEnabled)
    {
        switch (keyCode)
        {
        case InputEvents::KeyCode::KEY_DELETE:
        case InputEvents::KeyCode::KEY_KP_DELETE:
            if (_cursorPosition < (std::size_t)_charCount)
            {
                StringUtils::StringUTF8 stringUTF8;

                stringUTF8.replace(_inputText);
                stringUTF8.deleteChar(_cursorPosition);
                setCursorPosition(_cursorPosition);
                _charCount = stringUTF8.length();
                setString(stringUTF8.getAsCharSequence());
            }
            break;
        case InputEvents::KeyCode::KEY_LEFT_ARROW:
            if (_cursorPosition)
            {
                setCursorPosition(_cursorPosition - 1);
                updateCursorDisplayText();
            }
            break;
        case InputEvents::KeyCode::KEY_RIGHT_ARROW:
            if (_cursorPosition < (std::size_t)_charCount)
            {
                setCursorPosition(_cursorPosition + 1);
                updateCursorDisplayText();
            }
            break;
		case InputEvents::KeyCode::KEY_HOME:
		case InputEvents::KeyCode::KEY_KP_HOME:
		{
			// Get current row offset
            size_t cursorPosReal = _cursorPositionToCharPosition(this->getString().c_str(), _cursorPosition);
			std::size_t currentRowSearch = this->getString().rfind('\n', cursorPosReal <= 0 ? 0 : cursorPosReal - 1);
			int currentRowStart = currentRowSearch != std::string::npos ? currentRowSearch + 1 : 0;

			setCursorPosition(_charPositionToCursorPosition(this->getString().c_str(), currentRowStart));
			updateCursorDisplayText();
		}
			break;
		case InputEvents::KeyCode::KEY_END:
			if (_cursorPosition < (std::size_t)_charCount)
			{
				// Get current row offset
                size_t cursorPosReal = _cursorPositionToCharPosition(this->getString().c_str(), _cursorPosition);
				std::size_t nextRowSearch = this->getString().find('\n', cursorPosReal <= 0 ? 0 : cursorPosReal);
				int rowEnd = nextRowSearch != std::string::npos ? nextRowSearch : _charCount;

			    setCursorPosition(_charPositionToCursorPosition(this->getString().c_str(), rowEnd));
				updateCursorDisplayText();
			}
			break;
        case InputEvents::KeyCode::KEY_UP_ARROW:
            if (_cursorPosition)
            {
                // Get current row offset
                size_t cursorPosReal = _cursorPositionToCharPosition(this->getString().c_str(), _cursorPosition);
                std::size_t currentRowSearch = this->getString().rfind('\n', cursorPosReal <= 0 ? 0 : cursorPosReal - 1);
                int currentRowStart = currentRowSearch != std::string::npos ? currentRowSearch + 1 : 0;
                int currentRowOffset = cursorPosReal - currentRowStart;

                // Disallow moving cursor up if on the first row
                if (currentRowStart > 0)
                {
                    // Calculate length of previous row
                    std::size_t previousRowSearch = this->getString().rfind('\n', currentRowSearch <= 0 ? 0 : currentRowSearch - 1);
                    int previousRowStart = previousRowSearch != std::string::npos ? previousRowSearch + 1 : 0;
                    int previousRowLength = currentRowStart - previousRowStart;

                    // Determine new cursor offset for previous row
                    int newOffset = previousRowLength < currentRowOffset ? previousRowLength <= 0 ? 0 : previousRowLength - 1 : currentRowOffset;
                    int newCursorPos = previousRowStart + newOffset;

					// Rare edge case correction
					if (int(cursorPosReal) == newCursorPos && newCursorPos > 0)
					{
						newCursorPos--;
					}

			        setCursorPosition(_charPositionToCursorPosition(this->getString().c_str(), newCursorPos));
                    updateCursorDisplayText();
                }
            }
            break;
        case InputEvents::KeyCode::KEY_DOWN_ARROW:
            if (_cursorPosition < (std::size_t)_charCount)
            {
                // Get current row offset
                size_t cursorPosReal = _cursorPositionToCharPosition(this->getString().c_str(), _cursorPosition);
                std::size_t currentRowSearch = this->getString().rfind('\n', cursorPosReal <= 0 ? 0 : cursorPosReal - 1);
                int currentRowStart = currentRowSearch != std::string::npos ? currentRowSearch + 1 : 0;
                int currentRowOffset = cursorPosReal - currentRowStart;

                // Calculate length of next row
                std::size_t nextRowSearch = this->getString().find('\n', cursorPosReal <= 0 ? 0 : cursorPosReal);
                std::size_t nextRowEndSearch = this->getString().find('\n', nextRowSearch + 1);
                int nextRowStart = nextRowSearch != std::string::npos ? nextRowSearch + 1 : 0;
                int nextRowLength = nextRowEndSearch != std::string::npos ? nextRowEndSearch - nextRowStart : _charCount - nextRowStart;

                // Determine new cursor offset for next row
                int newOffset = nextRowLength < currentRowOffset ? nextRowLength <= 0 ? 0 : nextRowLength - 1 : currentRowOffset;
                int newCursorPos = nextRowStart + newOffset;

				// Rare edge case correction
				if (int(cursorPosReal) == newCursorPos && newCursorPos + 1 < int(_charCount))
				{
					newCursorPos++;
				}

                setCursorPosition(_charPositionToCursorPosition(this->getString().c_str(), newCursorPos));
                updateCursorDisplayText();
            }
            break;
        case InputEvents::KeyCode::KEY_ESCAPE:
            detachWithIME();
            break;
        default:
            break;
        }
    }
}

const std::string& TextFieldTTF::getString() const
{
    return _inputText;
}

// place holder text property
void TextFieldTTF::setPlaceHolder(const std::string& text)
{
    _placeHolder = text;
    if (_inputText.empty())
    {
        Label::setTextColor(_colorSpaceHolder);
        Label::setString(_placeHolder);
    }
}

const std::string& TextFieldTTF::getPlaceHolder() const
{
    return _placeHolder;
}

void TextFieldTTF::setCursorEnabled(bool enabled)
{
    if (_currentLabelType == LabelType::TTF)
    {
        if (_cursorEnabled != enabled)
        {
            _cursorEnabled = enabled;
            if (_cursorEnabled)
            {
                _cursorPosition = _charCount;
                scheduleUpdate();
            }
            else
            {
                _cursorPosition = 0;
                unscheduleUpdate();
            }
        }
    }
    else
    {
        CCLOG("TextFieldTTF cursor worked only LabelType::TTF");
    }
}

// secureTextEntry
void TextFieldTTF::setSecureTextEntry(bool value)
{
    if (_secureTextEntry != value)
    {
        _secureTextEntry = value;
        setString(_inputText);
    }
}

void TextFieldTTF::setPasswordTextStyle(const std::string &text)
{
    if (text.length() < 1)
    {
        return;
    }

    if (text != _passwordStyleText) {
        _passwordStyleText = text;
        setString(_inputText);
    }
}

const std::string& TextFieldTTF::getPasswordTextStyle() const
{
    return _passwordStyleText;
}

bool TextFieldTTF::isSecureTextEntry() const
{
    return _secureTextEntry;
}

NS_CC_END

