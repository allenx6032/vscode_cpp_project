// module.cpp - v1.0.1
/*
 *  Copyright (c) 2007 Leigh Johnston.
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 *     * Neither the name of Leigh Johnston nor the names of any
 *       other contributors to this software may be used to endorse or
 *       promote products derived from this software without specific prior
 *       written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <neolib/neolib.hpp>
#include <neolib/file.hpp>
#include <neolib/win32_module.hpp>

namespace neolib
{
	win32_module::win32_module(const std::string& aPath) : 
		iHandle(NULL) 
	{ 
		load(aPath); 
	}

	win32_module::~win32_module() 
	{ 
		unload(); 
	}

	bool win32_module::load(const std::string& aPath)
	{ 
		try
		{
			iHandle = ::LoadLibrary(neolib::convert_path(aPath).c_str());
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("neolib::win32_module: Failed to load module '" + aPath + "', reason: " + e.what());
		}
		catch (...)
		{
			throw std::runtime_error("neolib::win32_module: Failed to load module '" + aPath + "', unknown reason");
		}
		return loaded(); 
	}

	void win32_module::unload() 
	{ 
		::FreeLibrary(iHandle); iHandle = NULL; 
	}

	bool win32_module::loaded() const 
	{ 
		return iHandle != NULL; 
	}

	void* win32_module::procedure_address(const std::string& aProcedureName)
	{ 
		return ::GetProcAddress(iHandle, aProcedureName.c_str()); 
	}
}