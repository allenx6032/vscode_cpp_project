/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
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

#ifndef _CCFont_h_
#define _CCFont_h_

#include <string>
#include "cocos_CCLabel.h"

NS_CC_BEGIN

class FontAtlas;

// fwd
class FontAtlas;


class CC_DLL Font : public Ref
{
public:
    virtual  FontAtlas *createFontAtlas() = 0;

    virtual int* getHorizontalKerningForTextUTF16(const std::u16string& text, int &outNumLetters) const = 0;
    virtual const char* getCurrentGlyphCollection() const;
    
    
    virtual int getFontMaxHeight() const { return 0; }
    
protected:
    
    Font();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~Font() {}
    void setCurrentGlyphCollection(GlyphCollection glyphs, const char *customGlyphs = 0);
    const char * getGlyphCollection(GlyphCollection glyphs) const;
    

    GlyphCollection     _usedGlyphs;
    char              * _customGlyphs;
    static const char * _glyphASCII;
    static const char * _glyphNEHE;

};

NS_CC_END


#endif
