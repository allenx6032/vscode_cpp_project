/*
Copyright (c) 2011-2013 Steven Arnow
'darnit_dynlib.h' - This file is part of libdarnit

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/


#ifndef __DARNIT_DYNLIB_H__
#define	__DARNIT_DYNLIB_H__

#ifdef _WIN32
#define	DARNIT_EXPORT	__declspec(dllexport)
#else
#define	DARNIT_EXPORT	__attribute__ ((visibility ("default")))
#endif

typedef void DARNIT_DYNLIB;

DARNIT_DYNLIB *d_dynlib_open(const char *fname);
void *d_dynlib_get(DARNIT_DYNLIB *lib, const char *symbol);
DARNIT_DYNLIB *d_dynlib_close(DARNIT_DYNLIB *lib);

#endif
