/****************************************************************************
 *
 * ftcache.c
 *
 *   The FreeType Caching sub-system (body only).
 *
 * Copyright (C) 2000-2023 by
 * David Turner, Robert Wilhelm, and Werner Lemberg.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 *
 */


#define FT_MAKE_OPTION_SINGLE_OBJECT

#include "ftcbasic.c.h"
#include "ftccache.c.h"
#include "ftccmap.c.h"
#include "ftcglyph.c.h"
#include "ftcimage.c.h"
#include "ftcmanag.c.h"
#include "ftcmru.c.h"
#include "ftcsbits.c.h"


/* END */
