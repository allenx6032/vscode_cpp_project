/**
 * Copyright (c) 2006-2016 LOVE Development Team
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

#ifndef LOVE_IMAGE_MAGPIE_DDS_HANDLER_H
#define LOVE_IMAGE_MAGPIE_DDS_HANDLER_H

// LOVE
#include "love_magpie_CompressedFormatHandler.h"

// dds parser
#include "ddsparse.h"

// STL
#include <string>

namespace love
{
namespace image
{
namespace magpie
{

/**
 * Interface between CompressedImageData and the ddsparse library.
 **/
class DDSHandler : public CompressedFormatHandler
{
public:

	virtual ~DDSHandler() {}

	// Implements CompressedFormatHandler.
	virtual bool canParse(const filesystem::FileData *data);
	virtual uint8 *parse(filesystem::FileData *filedata, std::vector<CompressedImageData::SubImage> &images, size_t &dataSize, CompressedImageData::Format &format, bool &sRGB);

private:

	static CompressedImageData::Format convertFormat(dds::Format ddsformat, bool &sRGB);

}; // DDSHandler

} // magpie
} // image
} // love

#endif // LOVE_IMAGE_MAGPIE_DDS_HANDLER_H
