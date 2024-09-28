/***************************************************************************/
/*                                                                         */
/*  autofit.c                                                              */
/*                                                                         */
/*    Auto-fitter module (body).                                           */
/*                                                                         */
/*  Copyright 2003-2007, 2011, 2013 by                                     */
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
#include "ft2_afpic.c.h"
#include "ft2_afangles.c.h"
#include "ft2_afblue.c.h"
#include "ft2_afglobal.c.h"
#include "ft2_afhints.c.h"

#include "ft2_afranges.c.h"

#include "ft2_afdummy.c.h"
#include "ft2_aflatin.c.h"
#ifdef FT_OPTION_AUTOFIT2
#include "ft2_aflatin2.c.h"
#endif
#include "ft2_afcjk.c.h"
#include "ft2_afindic.c.h"

#include "ft2_hbshim.c.h"

#include "ft2_afloader.c.h"
#include "ft2_afmodule.c.h"

#ifdef AF_CONFIG_OPTION_USE_WARPER
#include "ft2_afwarp.c.h"
#endif

/* END */
