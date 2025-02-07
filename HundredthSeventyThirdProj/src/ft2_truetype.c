/****************************************************************************
 *
 * truetype.c
 *
 *   FreeType TrueType driver component (body only).
 *
 * Copyright (C) 1996-2023 by
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

#include "ft2_ttdriver.c.h"   /* driver interface    */
#include "ft2_ttgload.c.h"    /* glyph loader        */
#include "ft2_ttgxvar.c.h"    /* gx distortable font */
#include "ft2_ttinterp.c.h"
#include "ft2_ttobjs.c.h"     /* object manager      */
#include "ft2_ttpload.c.h"    /* tables loader       */


/* END */
