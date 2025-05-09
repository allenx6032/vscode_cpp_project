/***************************************************************************/
/*                                                                         */
/*  internal.h                                                             */
/*                                                                         */
/*    Internal header files (specification only).                          */
/*                                                                         */
/*  Copyright 1996-2004, 2013 by                                           */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* This file is automatically included by `ft2build.h'.                  */
  /* Do not include it manually!                                           */
  /*                                                                       */
  /*************************************************************************/


#define FT_INTERNAL_OBJECTS_H             <ft2_ftobjs.h>
#define FT_INTERNAL_PIC_H                 <ft2_ftpic.h>
#define FT_INTERNAL_STREAM_H              <ft2_ftstream.h>
#define FT_INTERNAL_MEMORY_H              <ft2_ftmemory.h>
#define FT_INTERNAL_DEBUG_H               <ft2_ftdebug.h>
#define FT_INTERNAL_CALC_H                <ft2_ftcalc.h>
#define FT_INTERNAL_DRIVER_H              <ft2_ftdriver.h>
#define FT_INTERNAL_TRACE_H               <ft2_fttrace.h>
#define FT_INTERNAL_GLYPH_LOADER_H        <ft2_ftgloadr.h>
#define FT_INTERNAL_SFNT_H                <ft2_sfnt.h>
#define FT_INTERNAL_SERVICE_H             <ft2_ftserv.h>
#define FT_INTERNAL_RFORK_H               <ft2_ftrfork.h>
#define FT_INTERNAL_VALIDATE_H            <ft2_ftvalid.h>

#define FT_INTERNAL_TRUETYPE_TYPES_H      <ft2_tttypes.h>
#define FT_INTERNAL_TYPE1_TYPES_H         <ft2_t1types.h>

#define FT_INTERNAL_POSTSCRIPT_AUX_H      <ft2_psaux.h>
#define FT_INTERNAL_POSTSCRIPT_HINTS_H    <ft2_pshints.h>
#define FT_INTERNAL_POSTSCRIPT_GLOBALS_H  <ft2_psglobal.h>

#define FT_INTERNAL_AUTOHINT_H            <ft2_autohint.h>


#if defined( _MSC_VER )      /* Visual C++ (and Intel C++) */

  /* We disable the warning `conditional expression is constant' here */
  /* in order to compile cleanly with the maximum level of warnings.  */
  /* In particular, the warning complains about stuff like `while(0)' */
  /* which is very useful in macro definitions.  There is no benefit  */
  /* in having it enabled.                                            */
#pragma warning( disable : 4127 )

#endif /* _MSC_VER */


/* END */
