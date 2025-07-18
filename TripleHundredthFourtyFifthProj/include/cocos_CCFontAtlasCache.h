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

#ifndef _CCFontAtlasCache_h_
#define _CCFontAtlasCache_h_

#include <unordered_map>

#include "cocos_CCLabel.h"

NS_CC_BEGIN

class FontAtlas;

class CC_DLL FontAtlasCache
{  
public:
    static FontAtlas * getFontAtlasTTF(const TTFConfig & config);
    static FontAtlas * getFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset = Vec2::ZERO);

    static FontAtlas * getFontAtlasCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
    static FontAtlas * getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
    static FontAtlas * getFontAtlasCharMap(const std::string& plistFile);
    
    static bool releaseFontAtlas(FontAtlas *atlas);

    /** Removes cached data.
     It will purge the textures atlas and if multiple texture exist in one FontAtlas.
     */
    static void purgeCachedData();
    
private: 
    static std::string generateFontName(const std::string& fontFileName, int size, GlyphCollection theGlyphs, bool useDistanceField);
    static std::unordered_map<std::string, FontAtlas *> _atlasMap;
};

NS_CC_END

#endif
