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

#include "ttdriver.c.h"   /* driver interface    */
#include "ttgload.c.h"    /* glyph loader        */
#include "ttgxvar.c.h"    /* gx distortable font */
#include "ttinterp.c.h"
#include "ttobjs.c.h"     /* object manager      */
#include "ttpload.c.h"    /* tables loader       */


/* END */
