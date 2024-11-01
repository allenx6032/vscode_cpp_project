/**
 * Copyright (c) 2006-2015 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#include "love_freetype_Font.h"

#include "love_freetype_TrueTypeRasterizer.h"
#include "love_ImageRasterizer.h"

#include "utf8.h"

#include <string.h>

namespace love
{
namespace font
{
namespace freetype
{

// Default TrueType font.
#include "love_Vera.ttf.h"

Font::Font()
{
	if (FT_Init_FreeType(&library))
		throw love::Exception("TrueTypeFont Loading error: FT_Init_FreeType failed");
}

Font::~Font()
{
	FT_Done_FreeType(library);
}

Rasterizer *Font::newRasterizer(int size)
{
	StrongRef<filesystem::FileData> data(new filesystem::FileData(sizeof(Vera_ttf), "Vera.ttf"));
	data->release();

	memcpy(data->getData(), Vera_ttf, sizeof(Vera_ttf));

	return new TrueTypeRasterizer(library, data.get(), size);
}

Rasterizer *Font::newRasterizer(Data *data, int size)
{
	return new TrueTypeRasterizer(library, data, size);
}

Rasterizer *Font::newRasterizer(love::image::ImageData *data, const std::string &text)
{
	size_t strlen = text.size();
	size_t numglyphs = 0;

	uint32 *glyphs = new uint32[strlen];

	try
	{
		utf8::iterator<std::string::const_iterator> i(text.begin(), text.begin(), text.end());
		utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

		while (i != end)
			glyphs[numglyphs++] = *i++;
	}
	catch (utf8::exception &e)
	{
		delete [] glyphs;
		throw love::Exception("UTF-8 decoding error: %s", e.what());
	}

	Rasterizer *r = newRasterizer(data, glyphs, numglyphs);
	delete [] glyphs;

	return r;
}

Rasterizer *Font::newRasterizer(love::image::ImageData *data, uint32 *glyphs, int numglyphs)
{
	return new ImageRasterizer(data, glyphs, numglyphs);
}

GlyphData *Font::newGlyphData(Rasterizer *r, const std::string &text)
{
	uint32 codepoint = 0;

	try
	{
		codepoint = utf8::peek_next(text.begin(), text.end());
	}
	catch (utf8::exception &e)
	{
		throw love::Exception("UTF-8 decoding error: %s", e.what());
	}

	return r->getGlyphData(codepoint);
}

GlyphData *Font::newGlyphData(Rasterizer *r, uint32 glyph)
{
	return r->getGlyphData(glyph);
}

const char *Font::getName() const
{
	return "love.font.freetype";
}

} // freetype
} // font
} // love
