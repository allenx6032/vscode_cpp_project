/*
Copyright (c) 2011-2013 Steven Arnow
'isometric.h' - This file is part of libdarnit

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


#ifndef __ISOMETRIC_H__
#define	__ISOMETRIC_H__


void isometricFromScreen(int s_x, int s_y, int o_x, int o_y, int w, int h, int *iso_x, int *iso_y);
void isometricToScreen(int x, int y, int o_x, int o_y, int w, int h, int *res_x, int *res_y);

#endif
