/***************************************************************************/
/*                                                                         */
/*  ftbase.c                                                               */
/*                                                                         */
/*    Single object library component (body only).                         */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2006, 2007, 2008, 2009 by       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include <ft2_build.h>

#define  FT_MAKE_OPTION_SINGLE_OBJECT

#include "ft2_ftpic.c.h"
#include "ft2_basepic.c.h"
#include "ft2_ftadvanc.c.h"
#include "ft2_ftcalc.c.h"
#include "ft2_ftdbgmem.c.h"
#include "ft2_ftgloadr.c.h"
#include "ft2_ftobjs.c.h"
#include "ft2_ftoutln.c.h"
#include "ft2_ftrfork.c.h"
#include "ft2_ftsnames.c.h"
#include "ft2_ftstream.c.h"
#include "ft2_fttrigon.c.h"
#include "ft2_ftutil.c.h"

#ifdef FT_MACINTOSH
#include "ft2_ftmac.c.h"
#endif

/* END */
