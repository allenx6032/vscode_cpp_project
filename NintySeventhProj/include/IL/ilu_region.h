//-----------------------------------------------------------------------------
//
// ImageLib Utility Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 07/09/2002 <--Y2K Compliant! =]
//
// Filename: src-ILU/src/IL/ilu_region.h
//
// Description: Creates an image region.
//
//-----------------------------------------------------------------------------

#ifndef ilu_REGION_H
#define ilu_REGION_H

#include "IL/ilu_internal.h"


typedef struct Edge
{
	ILint	yUpper;
	ILfloat	xIntersect, dxPerScan;
	struct	Edge *next;
} Edge;


#endif//IL/ilu_REGION_H

