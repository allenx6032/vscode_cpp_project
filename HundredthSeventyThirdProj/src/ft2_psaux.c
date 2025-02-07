/****************************************************************************
 *
 * psaux.c
 *
 *   FreeType auxiliary PostScript driver component (body only).
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

#include "ft2_afmparse.c.h"
#include "ft2_psauxmod.c.h"
#include "ft2_psconv.c.h"
#include "ft2_psobjs.c.h"
#include "ft2_t1cmap.c.h"
#include "ft2_t1decode.c.h"
#include "ft2_cffdecode.c.h"

#include "ft2_psarrst.c.h"
#include "ft2_psblues.c.h"
#include "ft2_pserror.c.h"
#include "ft2_psfont.c.h"
#include "ft2_psft.c.h"
#include "ft2_pshints2.c.h"
#include "ft2_psintrp.c.h"
#include "ft2_psread.c.h"
#include "ft2_psstack.c.h"


/* END */
