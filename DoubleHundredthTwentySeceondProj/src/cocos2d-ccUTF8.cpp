/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#include "cocos2d-CCUTF8.h"
#include "cocos2d-CCCommon.h"
#include "cocos2d-CCConsole.h"
#include "ConvertUTF.h"
#include <limits>

NS_CC_BEGIN

namespace StringUtils {

/*--- This a C++ universal sprintf in the future.
**  @pitfall: The behavior of vsnprintf between VS2013 and VS2015/2017 is different
**      VS2013 or Unix-Like System will return -1 when buffer not enough, but VS2015/2017 will return the actural needed length for buffer at this station
**      The _vsnprintf behavior is compatible API which always return -1 when buffer isn't enough at VS2013/2015/2017
**      Yes, The vsnprintf is more efficient implemented by MSVC 19.0 or later, AND it's also standard-compliant, see reference: http://www.cplusplus.com/reference/cstdio/vsnprintf/
*/
std::string format(const char* format, ...)
{
#define CC_VSNPRINTF_BUFFER_LENGTH 512
    va_list args;
    std::string buffer(CC_VSNPRINTF_BUFFER_LENGTH, '\0');

    va_start(args, format);
    int nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
    va_end(args);

    if (nret >= 0) {
        if ((unsigned int)nret < buffer.length()) {
            buffer.resize(nret);
        }
        else if ((unsigned int)nret > buffer.length()) { // VS2015/2017 or later Visual Studio Version
            buffer.resize(nret);

            va_start(args, format);
            nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
            va_end(args);

            assert(nret == buffer.length());
        }
        // else equals, do nothing.
    }
    else { // less or equal VS2013 and Unix System glibc implement.
        do {
            buffer.resize(buffer.length() * 3 / 2);

            va_start(args, format);
            nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
            va_end(args);

        } while (nret < 0);

        buffer.resize(nret);
    }

    return buffer;
}

/*
 * @str:    the string to search through.
 * @c:        the character to not look for.
 *
 * Return value: the index of the last character that is not c.
 * */
unsigned int getIndexOfLastNotChar16(const std::vector<char16_t>& str, char16_t c)
{
    int len = static_cast<int>(str.size());

    int i = len - 1;
    for (; i >= 0; --i)
        if (str[i] != c) return i;

    return i;
}

/*
 * @str:    the string to trim
 * @index:    the index to start trimming from.
 *
 * Trims str st str=[0, index) after the operation.
 *
 * Return value: the trimmed string.
 * */
static void trimUTF16VectorFromIndex(std::vector<char16_t>& str, int index)
{
    int size = static_cast<int>(str.size());
    if (index >= size || index < 0)
        return;

    str.erase(str.begin() + index, str.begin() + size);
}
    
/*
 * @str:    the string to trim
 * @index:    the index to start trimming from.
 *
 * Trims str st str=[0, index) after the operation.
 *
 * Return value: the trimmed string.
 * */
static void trimUTF32VectorFromIndex(std::vector<char32_t>& str, int index)
{
    int size = static_cast<int>(str.size());
    if (index >= size || index < 0)
        return;
    
    str.erase(str.begin() + index, str.begin() + size);
}

/*
 * @ch is the unicode character whitespace?
 *
 * Reference: http://en.wikipedia.org/wiki/Whitespace_character#Unicode
 *
 * Return value: weather the character is a whitespace character.
 * */
bool isUnicodeSpace(char32_t ch)
{
    return  (ch >= 0x0009 && ch <= 0x000D) || ch == 0x0020 || ch == 0x0085 || ch == 0x00A0 || ch == 0x1680
    || (ch >= 0x2000 && ch <= 0x200A) || ch == 0x2028 || ch == 0x2029 || ch == 0x202F
    ||  ch == 0x205F || ch == 0x3000;
}

bool isCJKUnicode(char32_t ch)
{
    return (ch >= 0x4E00 && ch <= 0x9FBF)   // CJK Unified Ideographs
        || (ch >= 0x2E80 && ch <= 0x2FDF)   // CJK Radicals Supplement & Kangxi Radicals
        || (ch >= 0x2FF0 && ch <= 0x30FF)   // Ideographic Description Characters, CJK Symbols and Punctuation & Japanese
        || (ch >= 0x3100 && ch <= 0x31BF)   // Korean
        || (ch >= 0xAC00 && ch <= 0xD7AF)   // Hangul Syllables
        || (ch >= 0xF900 && ch <= 0xFAFF)   // CJK Compatibility Ideographs
        || (ch >= 0xFE30 && ch <= 0xFE4F)   // CJK Compatibility Forms
        || (ch >= 0x31C0 && ch <= 0x4DFF)   // Other extensions
        || (ch >= 0x1f004 && ch <= 0x1f682);// Emoji
}
    
bool isUnicodeNonBreaking(char32_t ch)
{
    return ch == 0x00A0   // Non-Breaking Space
    || ch == 0x202F       // Narrow Non-Breaking Space
    || ch == 0x2007       // Figure Space
    || ch == 0x2060;      // Word Joiner
}
    
void trimUTF16Vector(std::vector<char16_t>& str)
{
    int len = static_cast<int>(str.size());

    if ( len <= 0 )
        return;

    int last_index = len - 1;

    // Only start trimming if the last character is whitespace..
    if (isUnicodeSpace(str[last_index]))
    {
        for (int i = last_index - 1; i >= 0; --i)
        {
            if (isUnicodeSpace(str[i]))
                last_index = i;
            else
                break;
        }

        trimUTF16VectorFromIndex(str, last_index);
    }
}
    
void trimUTF32Vector(std::vector<char32_t>& str)
{
    int len = static_cast<int>(str.size());
    
    if ( len <= 0 )
        return;
    
    int last_index = len - 1;
    
    // Only start trimming if the last character is whitespace..
    if (isUnicodeSpace(str[last_index]))
    {
        for (int i = last_index - 1; i >= 0; --i)
        {
            if (isUnicodeSpace(str[i]))
                last_index = i;
            else
                break;
        }
        
        trimUTF32VectorFromIndex(str, last_index);
    }
}


template <typename T>
struct ConvertTrait {
    typedef T ArgType;
};
template <>
struct ConvertTrait<char> {
    typedef UTF8 ArgType;
};
template <>
struct ConvertTrait<char16_t> {
    typedef UTF16 ArgType;
};
template <>
struct ConvertTrait<char32_t> {
    typedef UTF32 ArgType;
};

template <typename From, typename To, typename FromTrait = ConvertTrait<From>, typename ToTrait = ConvertTrait<To>>
bool utfConvert(
    const std::basic_string<From>& from, std::basic_string<To>& to,
    ConversionResult(*cvtfunc)(const typename FromTrait::ArgType**, const typename FromTrait::ArgType*,
        typename ToTrait::ArgType**, typename ToTrait::ArgType*,
        ConversionFlags)
    )
{
    static_assert(sizeof(From) == sizeof(typename FromTrait::ArgType), "Error size mismatched");
    static_assert(sizeof(To) == sizeof(typename ToTrait::ArgType), "Error size mismatched");

    if (from.empty())
    {
        to.clear();
        return true;
    }

    // See: http://unicode.org/faq/utf_bom.html#gen6
    static const int most_bytes_per_character = 4;

    const size_t maxNumberOfChars = from.length(); // all UTFs at most one element represents one character.
    const size_t numberOfOut = maxNumberOfChars * most_bytes_per_character / sizeof(To);

    std::basic_string<To> working(numberOfOut, 0);

    auto inbeg = reinterpret_cast<const typename FromTrait::ArgType*>(&from[0]);
    auto inend = inbeg + from.length();


    auto outbeg = reinterpret_cast<typename ToTrait::ArgType*>(&working[0]);
    auto outend = outbeg + working.length();
    auto r = cvtfunc(&inbeg, inend, &outbeg, outend, strictConversion);
    if (r != conversionOK)
        return false;

    working.resize(reinterpret_cast<To*>(outbeg) - &working[0]);
    to = std::move(working);

    return true;
};


bool UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16)
{
    return utfConvert(utf8, outUtf16, ConvertUTF8toUTF16);
}

bool UTF8ToUTF32(const std::string& utf8, std::u32string& outUtf32)
{
    return utfConvert(utf8, outUtf32, ConvertUTF8toUTF32);
}

bool UTF16ToUTF8(const std::u16string& utf16, std::string& outUtf8)
{
    return utfConvert(utf16, outUtf8, ConvertUTF16toUTF8);
}
    
bool UTF16ToUTF32(const std::u16string& utf16, std::u32string& outUtf32)
{
    return utfConvert(utf16, outUtf32, ConvertUTF16toUTF32);
}

bool UTF32ToUTF8(const std::u32string& utf32, std::string& outUtf8)
{
    return utfConvert(utf32, outUtf8, ConvertUTF32toUTF8);
}

bool UTF32ToUTF16(const std::u32string& utf32, std::u16string& outUtf16)
{
    return utfConvert(utf32, outUtf16, ConvertUTF32toUTF16);
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) 
std::string getStringUTFCharsJNI(JNIEnv* env, jstring srcjStr, bool* ret)
{
    std::string utf8Str;
    if(srcjStr != nullptr)
    {
        const unsigned short * unicodeChar = ( const unsigned short *)env->GetStringChars(srcjStr, nullptr);
        size_t unicodeCharLength = env->GetStringLength(srcjStr);
        const std::u16string unicodeStr((const char16_t *)unicodeChar, unicodeCharLength);
        bool flag = UTF16ToUTF8(unicodeStr, utf8Str);
        if (ret)
        {
            *ret = flag;
        }
        if (!flag)
        {
            utf8Str = "";
        }
        env->ReleaseStringChars(srcjStr, unicodeChar);
    }
    else
    {
        if (ret)
        {
            *ret = false;
        }
        utf8Str = "";
    }
    return utf8Str;
}

jstring newStringUTFJNI(JNIEnv* env, const std::string& utf8Str, bool* ret)
{
    std::u16string utf16Str;
    bool flag = cocos2d::StringUtils::UTF8ToUTF16(utf8Str, utf16Str);

    if (ret)
    {
        *ret = flag;
    }

    if(!flag)
    {
        utf16Str.clear();
    }
    jstring stringText = env->NewString((const jchar*)utf16Str.data(), utf16Str.length());
    return stringText;
}
#endif

std::vector<char16_t> getChar16VectorFromUTF16String(const std::u16string& utf16)
{
    return std::vector<char16_t>(utf16.begin(), utf16.end());
}

long getCharacterCountInUTF8String(const std::string& utf8)
{
    return getUTF8StringLength((const UTF8*)utf8.c_str());
}


StringUTF8::StringUTF8()
{

}

StringUTF8::StringUTF8(const std::string& newStr)
{
    replace(newStr);
}

StringUTF8::~StringUTF8()
{

}

std::size_t StringUTF8::length() const
{
    return _str.size();
}

void StringUTF8::replace(const std::string& newStr)
{
    _str.clear();
    if (!newStr.empty())
    {
        UTF8* sequenceUtf8 = (UTF8*)newStr.c_str();

        int lengthString = getUTF8StringLength(sequenceUtf8);

        if (lengthString == 0)
        {
            CCLOG("Bad utf-8 set string: %s", newStr.c_str());
            return;
        }

        while (*sequenceUtf8)
        {
            std::size_t lengthChar = getNumBytesForUTF8(*sequenceUtf8);

            CharUTF8 charUTF8;
            charUTF8._char.append((char*)sequenceUtf8, lengthChar);
            sequenceUtf8 += lengthChar;

            _str.push_back(charUTF8);
        }
    }
}

std::string StringUTF8::getAsCharSequence() const
{
    return getAsCharSequence(0, std::numeric_limits<std::size_t>::max());
}

std::string StringUTF8::getAsCharSequence(std::size_t pos) const
{
    return getAsCharSequence(pos, std::numeric_limits<std::size_t>::max());
}

std::string StringUTF8::getAsCharSequence(std::size_t pos, std::size_t len) const
{
    std::string charSequence;
    std::size_t maxLen = _str.size() - pos;
    if (len > maxLen)
    {
        len = maxLen;
    }

    std::size_t endPos = len + pos;
    while (pos < endPos)
    {
        charSequence.append(_str[pos++]._char);
    }

    return charSequence;
}

bool StringUTF8::deleteChar(std::size_t pos)
{
    if (pos < _str.size())
    {
        _str.erase(_str.begin() + pos);
        return true;
    }
    else
    {
        return false;
    }
}

bool StringUTF8::insert(std::size_t pos, const std::string& insertStr)
{
    StringUTF8 utf8(insertStr);

    return insert(pos, utf8);
}

bool StringUTF8::insert(std::size_t pos, const StringUTF8& insertStr)
{
    if (pos <= _str.size())
    {
        _str.insert(_str.begin() + pos, insertStr._str.begin(), insertStr._str.end());

        return true;
    }
    else
    {
        return false;
    }
}

} //namespace StringUtils {

namespace {
    inline int wcslen_internal(const unsigned short* str)
    {
        if (str == nullptr)
            return -1;
        int i=0;
        while(*str++) i++;
        return i;
    }
}


int cc_wcslen(const unsigned short* str)
{
    return wcslen_internal(str);
}

void cc_utf8_trim_ws(std::vector<unsigned short>* str)
{
    if (str == nullptr)
        return;
    // unsigned short and char16_t are both 2 bytes
    std::vector<char16_t>* ret = reinterpret_cast<std::vector<char16_t>*>(str);
    StringUtils::trimUTF16Vector(*ret);
}

bool isspace_unicode(unsigned short ch)
{
    return StringUtils::isUnicodeSpace(static_cast<char32_t>(ch));
}


bool iscjk_unicode(unsigned short ch)
{
    return StringUtils::isCJKUnicode(static_cast<char32_t>(ch));
}


long cc_utf8_strlen (const char * p, int /*max*/)
{
    if (p == nullptr)
        return -1;
    return StringUtils::getCharacterCountInUTF8String(p);
}

unsigned int cc_utf8_find_last_not_char(const std::vector<unsigned short>& str, unsigned short c)
{
    std::vector<char16_t> char16Vector;
    for (const auto& e : str)
    {
        char16Vector.push_back(e);
    }
    
    return StringUtils::getIndexOfLastNotChar16(char16Vector, c);
}

std::vector<unsigned short> cc_utf16_vec_from_utf16_str(const unsigned short* str)
{
    std::vector<unsigned short> str_new;
    
    if (str == nullptr)
        return str_new;
    
    int len = wcslen_internal(str);
    
    for (int i = 0; i < len; ++i)
    {
        str_new.push_back(str[i]);
    }
    return str_new;
}

unsigned short* cc_utf8_to_utf16(const char* str_old, int length/* = -1*/, int* rUtf16Size/* = nullptr*/)
{
    if (str_old == nullptr)
        return nullptr;
    
    unsigned short* ret = nullptr;
    
    std::u16string outUtf16;
    std::string inUtf8 = length == -1 ? std::string(str_old) : std::string(str_old, length);
    bool succeed = StringUtils::UTF8ToUTF16(inUtf8, outUtf16);
    
    if (succeed)
    {
        ret = new (std::nothrow) unsigned short[outUtf16.length() + 1];
        ret[outUtf16.length()] = 0;
        memcpy(ret, outUtf16.data(), outUtf16.length() * sizeof(unsigned short));
        if (rUtf16Size)
        {
            *rUtf16Size = static_cast<int>(outUtf16.length());
        }
    }
    
    return ret;
}

char * cc_utf16_to_utf8 (const unsigned short  *str,
                  int             len,
                  long            * /*items_read*/,
                  long            * /*items_written*/)
{
    if (str == nullptr)
        return nullptr;
    
    
    std::u16string utf16;
    int utf16Len = len < 0 ? wcslen_internal(str) : len;
    
    for (int i = 0; i < utf16Len; ++i)
    {
        utf16.push_back(str[i]);
    }
    
    char* ret = nullptr;
    std::string outUtf8;
    bool succeed = StringUtils::UTF16ToUTF8(utf16, outUtf8);
    
    if (succeed)
    {
        ret = new (std::nothrow) char[outUtf8.length() + 1];
        ret[outUtf8.length()] = '\0';
        memcpy(ret, outUtf8.data(), outUtf8.length());
    }
    
    return ret;
}


NS_CC_END
