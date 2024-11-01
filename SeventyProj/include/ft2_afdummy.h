/***************************************************************************/
/*                                                                         */
/*  afdummy.h                                                              */
/*                                                                         */
/*    Auto-fitter dummy routines to be used if no hinting should be        */
/*    performed (specification).                                           */
/*                                                                         */
/*  Copyright 2003-2005, 2011, 2013 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __AFDUMMY_H__
#define __AFDUMMY_H__

#include "ft2_aftypes.h"


FT_BEGIN_HEADER

  /* A dummy writing system used when no hinting should be performed. */

  AF_DECLARE_WRITING_SYSTEM_CLASS( af_dummy_writing_system_class )

/* */

FT_END_HEADER


#endif /* __AFDUMMY_H__ */


/* END */
