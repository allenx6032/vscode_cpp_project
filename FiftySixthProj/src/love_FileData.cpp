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

#include "love_FileData.h"

// STD
#include <iostream>

namespace love
{
namespace filesystem
{
	FileData::FileData(int size, const std::string & filename)
		: data(new char[size]), size(size), filename(filename)
	{
		if(filename.rfind('.') != std::string::npos)
			extension = filename.substr(filename.rfind('.')+1);
	}

	FileData::~FileData()
	{
		delete [] data;
	}

	void * FileData::getData() const
	{
		return (void*)data;
	}

	int FileData::getSize() const
	{
		return size;
	}

	const std::string & FileData::getFilename() const
	{
		return filename;
	}

	const std::string & FileData::getExtension() const
	{
		return extension;
	}

} // filesystem
} // love
