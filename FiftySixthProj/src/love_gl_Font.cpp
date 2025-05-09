/**
* Copyright (c) 2006-2010 LOVE Development Team
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

#include "love_gl_Font.h"

#include <love_math.h>
#include <math.h>

namespace love
{
namespace graphics
{
namespace opengl
{
	
	Font::Font(love::font::FontData * data)
	: height(data->getHeight()), lineHeight(1.25), mSpacing(1)
	{
		glyphs = new Glyph*[MAX_CHARS];
		for(unsigned int i = 0; i < MAX_CHARS; i++)
		{
			glyphs[i] = new Glyph(data->getGlyphData(i));
			glyphs[i]->load();
			widths[i] = data->getGlyphData(i)->getWidth();
			spacing[i] = data->getGlyphData(i)->getAdvance();
			bearingX[i] = data->getGlyphData(i)->getBearingX();
			bearingY[i] = data->getGlyphData(i)->getBearingY();
		}
	}

	Font::~Font()
	{
		delete[] glyphs;
	}

	float Font::getHeight() const
	{
		return height / lineHeight;
	}
	
	void Font::print(std::string text, float x, float y) const
	{
		print(text, x, y, 0.0f, 1.0f, 1.0f);
	}
	
	void Font::print(std::string text, float x, float y, float angle, float sx, float sy) const
	{
		glPushMatrix();
		
		glTranslatef(ceil(x), ceil(y), 0.0f);
		glRotatef(LOVE_TODEG(angle), 0, 0, 1.0f);
		glScalef(sx, sy, 1.0f);
		int first = (int)text[0];
		int s = -bearingX[first];
		for (unsigned int i = 0; i < text.size(); i++) {
			int g = (int)text[i];
			if (!glyphs[g]) g = 32; // space
			glyphs[g]->draw(bearingX[g] + s, -bearingY[g]+height, 0, 1, 1, 0, 0);
			s += spacing[g] * mSpacing;
		}
		glPopMatrix();
	}
	
	void Font::print(char character, float x, float y) const
	{
		if (!glyphs[character]) character = ' ';
		glyphs[character]->draw(x, y+height, 0, 1, 1, 0, 0);
	}
	
	int Font::getWidth(const std::string & line) const
	{
		if(line.size() == 0) return 0;
		int temp = 0;

		for(unsigned int i = 0; i < line.size(); i++)
		{
			temp += (spacing[(int)line[i]] * mSpacing);
		}
		
		return temp;
	}

	int Font::getWidth(const char * line) const
	{
		return this->getWidth(std::string(line));
	}
	
	int Font::getWidth(const char character) const
	{
		return spacing[(int)character];
	}

	void Font::setLineHeight(float height)
	{
		this->lineHeight = height;
	}

	float Font::getLineHeight() const
	{
		return lineHeight;
	}

	void Font::setSpacing(float amount)
	{
		mSpacing = amount;
	}

	float Font::getSpacing() const
	{
		return mSpacing;
	}

} // opengl
} // graphics
} // love
