/***************************************************************************/
/*                                                                         */
/*  ftcache.c                                                              */
/*                                                                         */
/*    The FreeType Caching sub-system (body only).                         */
/*                                                                         */
/*  Copyright 2000-2001, 2003 by                                           */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#define FT_MAKE_OPTION_SINGLE_OBJECT

#include <ft2_build.h>
#include "ft2_ftcmru.c.h"
#include "ft2_ftcmanag.c.h"
#include "ft2_ftccache.c.h"
#include "ft2_ftccmap.c.h"
#include "ft2_ftcglyph.c.h"
#include "ft2_ftcimage.c.h"
#include "ft2_ftcsbits.c.h"
#include "ft2_ftcbasic.c.h"

/* END */
