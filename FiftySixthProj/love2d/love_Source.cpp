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

#include "Source.h"

namespace love
{
namespace audio
{
	Source::Source(Type type)
		: type(type)
	{
	}

	Source::~Source()
	{
	}

	bool Source::getConstant(const char * in, Type & out)
	{
		return types.find(in, out);
	}

	bool Source::getConstant(Type in, const char *& out)
	{
		return types.find(in, out);
	}

	StringMap<Source::Type, Source::TYPE_MAX_ENUM>::Entry Source::typeEntries[] =
	{
		{"static", Source::TYPE_STATIC},
		{"stream", Source::TYPE_STREAM},
	};

	StringMap<Source::Type, Source::TYPE_MAX_ENUM> Source::types(Source::typeEntries, sizeof(Source::typeEntries));

} // audio
} // love
