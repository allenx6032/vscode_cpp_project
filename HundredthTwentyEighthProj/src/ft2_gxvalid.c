/***************************************************************************/
/*                                                                         */
/*  gxvalid.c                                                              */
/*                                                                         */
/*    FreeType validator for TrueTypeGX/AAT tables (body only).            */
/*                                                                         */
/*  Copyright 2005 by suzuki toshiya, Masatake YAMATO, Red Hat K.K.,       */
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

#include "ft2_gxvfeat.c.h"
#include "ft2_gxvcommn.c.h"
#include "ft2_gxvbsln.c.h"
#include "ft2_gxvtrak.c.h"
#include "ft2_gxvjust.c.h"
#include "ft2_gxvmort.c.h"
#include "ft2_gxvmort0.c.h"
#include "ft2_gxvmort1.c.h"
#include "ft2_gxvmort2.c.h"
#include "ft2_gxvmort4.c.h"
#include "ft2_gxvmort5.c.h"
#include "ft2_gxvmorx.c.h"
#include "ft2_gxvmorx0.c.h"
#include "ft2_gxvmorx1.c.h"
#include "ft2_gxvmorx2.c.h"
#include "ft2_gxvmorx4.c.h"
#include "ft2_gxvmorx5.c.h"
#include "ft2_gxvkern.c.h"
#include "ft2_gxvopbd.c.h"
#include "ft2_gxvprop.c.h"
#include "ft2_gxvlcar.c.h"
#include "ft2_gxvmod.c.h"


/* END */
