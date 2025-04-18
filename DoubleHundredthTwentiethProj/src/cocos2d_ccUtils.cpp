/****************************************************************************
Copyright (c) 2010      cocos2d-x.org
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

#include "cocos2d_ccUtils.h"

#include <cmath>
#include <stdlib.h>

#include "cocos2d_CCSprite.h"
#include "cocos2d_CCRenderTexture.h"
#include "cocos2d_CCConsole.h"
#include "cocos2d_CCDirector.h"
#include "cocos2d_CCAsyncTaskPool.h"
#include "cocos2d_CCEventDispatcher.h"
#include "cocos2d_base64.h"
#include "cocos2d_CCImage.h"
#include "cocos2d_CCFileUtils.h"
#include "cocos2d_CCCustomCommand.h"
#include "cocos2d_CCRenderer.h"
#include "cocos2d_CCTextureCache.h"

NS_CC_BEGIN

int ccNextPOT(int x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

namespace utils
{
#define MAX_ITOA_BUFFER_SIZE 256
double atof(const char* str)
{
    if (str == nullptr)
    {
        return 0.0;
    }
    
    char buf[MAX_ITOA_BUFFER_SIZE];
    strncpy(buf, str, MAX_ITOA_BUFFER_SIZE);
    
    // strip string, only remain 7 numbers after '.'
    char* dot = strchr(buf, '.');
    if (dot != nullptr && dot - buf + 8 <  MAX_ITOA_BUFFER_SIZE)
    {
        dot[8] = '\0';
    }
    
    return ::atof(buf);
}

double gettime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return (double)tv.tv_sec + (double)tv.tv_usec/1000000;
}

long long getTimeInMilliseconds()
{
    struct timeval tv;
    gettimeofday (&tv, nullptr);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

CRect getCascadeBoundingBox(Node *node)
{
    CRect cbb;
    CSize contentSize = node->getContentSize();
    
    // check all children bounding box, get maximize box
    Node* child = nullptr;
    bool merge = false;
    for(auto object : node->getChildren())
    {
        child = dynamic_cast<Node*>(object);
        if (!child->isVisible()) continue;
        
        const CRect box = getCascadeBoundingBox(child);
        if (box.size.width <= 0 || box.size.height <= 0) continue;
        
        if (!merge)
        {
            cbb = box;
            merge = true;
        }
        else
        {
            cbb.merge(box);
        }
    }
    
    // merge content size
    if (contentSize.width > 0 && contentSize.height > 0)
    {
        const CRect box = RectApplyAffineTransform(CRect(0, 0, contentSize.width, contentSize.height), node->getNodeToWorldAffineTransform());
        if (!merge)
        {
            cbb = box;
        }
        else
        {
            cbb.merge(box);
        }
    }
    
    return cbb;
}

Sprite* createSpriteFromBase64Cached(const char* base64String, const char* key)
{
    Texture2D* texture = Director::getInstance()->getTextureCache()->getTextureForKey(key);

    if (texture == nullptr)
    {
        unsigned char* decoded;
        int length = base64Decode((const unsigned char*)base64String, (unsigned int)strlen(base64String), &decoded);

        Image *image = new (std::nothrow) Image();
        bool imageResult = image->initWithImageData(decoded, length);
        CCASSERT(imageResult, "Failed to create image from base64!");
        free(decoded);

        if (!imageResult) {
            CC_SAFE_RELEASE_NULL(image);
            return nullptr;
        }

        texture = Director::getInstance()->getTextureCache()->addImage(image, key);
        image->release();
    }

    Sprite* sprite = Sprite::createWithTexture(texture);
    
    return sprite;
}

Sprite* createSpriteFromBase64(const char* base64String)
{
    unsigned char* decoded;
    int length = base64Decode((const unsigned char*)base64String, (unsigned int)strlen(base64String), &decoded);

    Image *image = new (std::nothrow) Image();
    bool imageResult = image->initWithImageData(decoded, length);
    CCASSERT(imageResult, "Failed to create image from base64!");
    free(decoded);

    if (!imageResult) {
        CC_SAFE_RELEASE_NULL(image);
        return nullptr;
    }

    Texture2D *texture = new (std::nothrow) Texture2D();
    texture->initWithImage(image);
    texture->setAliasTexParameters();
    image->release();

    Sprite* sprite = Sprite::createWithTexture(texture);
    texture->release();

    return sprite;
}

LanguageType getLanguageTypeByISO2(const char* code)
{
    // this function is used by all platforms to get system language
    // except windows: cocos/platform/win32/CCApplication-win32.cpp
    LanguageType ret = LanguageType::ENGLISH;

    if (strncmp(code, "ar", 2) == 0)
    {
        ret = LanguageType::ARABIC;
    }
    else if (strncmp(code, "bg", 2) == 0)
    {
        ret = LanguageType::BULGARIAN;
    }
    else if (strncmp(code, "zh-CN", 5) == 0)
    {
        ret = LanguageType::CHINESE_SIMPLIFIED;
    }
    else if (strncmp(code, "zh-TW", 5) == 0)
    {
        ret = LanguageType::CHINESE_TRADITIONAL;
    }
    else if (strncmp(code, "cs", 2) == 0)
    {
        ret = LanguageType::CZECH;
    }
    else if (strncmp(code, "da", 2) == 0)
    {
        ret = LanguageType::DANISH;
    }
    else if (strncmp(code, "nl", 2) == 0)
    {
        ret = LanguageType::DUTCH;
    }
    else if (strncmp(code, "en", 2) == 0)
    {
        ret = LanguageType::ENGLISH;
    }
    else if (strncmp(code, "fi", 2) == 0)
    {
        ret = LanguageType::FINNISH;
    }
    else if (strncmp(code, "fr", 2) == 0)
    {
        ret = LanguageType::FRENCH;
    }
    else if (strncmp(code, "de", 2) == 0)
    {
        ret = LanguageType::GERMAN;
    }
    else if (strncmp(code, "el", 2) == 0)
    {
        ret = LanguageType::GREEK;
    }
    else if (strncmp(code, "hu", 2) == 0)
    {
        ret = LanguageType::HUNGARIAN;
    }
    else if (strncmp(code, "it", 2) == 0)
    {
        ret = LanguageType::ITALIAN;
    }
    else if (strncmp(code, "ja", 2) == 0)
    {
        ret = LanguageType::JAPANESE;
    }
    else if (strncmp(code, "ko", 2) == 0)
    {
        ret = LanguageType::KOREAN;
    }
    else if (strncmp(code, "no", 2) == 0)
    {
        ret = LanguageType::NORWEGIAN;
    }
    else if (strncmp(code, "pl", 2) == 0)
    {
        ret = LanguageType::BULGARIAN;
    }
    else if (strncmp(code, "pt", 2) == 0)
    {
        ret = LanguageType::PORTUGUESE;
    }
    else if (strncmp(code, "pt-BR", 5) == 0)
    {
        ret = LanguageType::PORTUGUESE_BRAZIL;
    }
    else if (strncmp(code, "ro", 2) == 0)
    {
        ret = LanguageType::ROMANIAN;
    }
    else if (strncmp(code, "ru", 2) == 0)
    {
        ret = LanguageType::RUSSIAN;
    }
    else if (strncmp(code, "es", 2) == 0)
    {
        ret = LanguageType::SPANISH;
    }
    else if (strncmp(code, "sv", 2) == 0)
    {
        ret = LanguageType::SWEDISH;
    }
    else if (strncmp(code, "th", 2) == 0)
    {
        ret = LanguageType::THAI;
    }
    else if (strncmp(code, "tr", 2) == 0)
    {
        ret = LanguageType::TURKISH;
    }
    else if (strncmp(code, "uk", 2) == 0)
    {
        ret = LanguageType::UKRAINIAN;
    }

    return ret;
}

}

NS_CC_END
