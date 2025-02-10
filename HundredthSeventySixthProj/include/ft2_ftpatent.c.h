/****************************************************************************
 *
 * ftpatent.c
 *
 *   FreeType API for checking patented TrueType bytecode instructions
 *   (body).  Obsolete, retained for backward compatibility.
 *
 * Copyright (C) 2007-2023 by
 * David Turner.
 *
 * This file is part of the FreeType project, and may only be used,
 * modified, and distributed under the terms of the FreeType project
 * license, LICENSE.TXT.  By continuing to use, modify, or distribute
 * this file you indicate that you have read the license and
 * understand and accept it fully.
 *
 */

#include <freetype.h>
#include <ft2_tttags.h>
#include <ft2_ftobjs.h>
#include <ft2_ftstream.h>
#include <svsfnt.h>
#include <svttglyf.h>


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Bool )
  FT_Face_CheckTrueTypePatents( FT_Face  face )
  {
    FT_UNUSED( face );

    return FALSE;
  }


  /* documentation is in freetype.h */

  FT_EXPORT_DEF( FT_Bool )
  FT_Face_SetUnpatentedHinting( FT_Face  face,
                                FT_Bool  value )
  {
    FT_UNUSED( face );
    FT_UNUSED( value );

    return FALSE;
  }

/* END */
