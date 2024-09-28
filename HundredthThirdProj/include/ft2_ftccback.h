/***************************************************************************/
/*                                                                         */
/*  ftccback.h                                                             */
/*                                                                         */
/*    Callback functions of the caching sub-system (specification only).   */
/*                                                                         */
/*  Copyright 2004-2006, 2011, 2013 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

#ifndef __FTCCBACK_H__
#define __FTCCBACK_H__

#include <ft2_build.h>
#include FT_CACHE_H
#include "ft2_ftcmru.h"
#include "ft2_ftcimage.h"
#include "ft2_ftcmanag.h"
#include "ft2_ftcglyph.h"
#include "ft2_ftcsbits.h"


  FT_LOCAL( void )
  ftc_inode_free( FTC_Node   inode,
                  FTC_Cache  cache );

  FT_LOCAL( FT_Error )
  ftc_inode_new( FTC_Node   *pinode,
                 FT_Pointer  gquery,
                 FTC_Cache   cache );

  FT_LOCAL( FT_Offset )
  ftc_inode_weight( FTC_Node   inode,
                    FTC_Cache  cache );


  FT_LOCAL( void )
  ftc_snode_free( FTC_Node   snode,
                  FTC_Cache  cache );

  FT_LOCAL( FT_Error )
  ftc_snode_new( FTC_Node   *psnode,
                 FT_Pointer  gquery,
                 FTC_Cache   cache );

  FT_LOCAL( FT_Offset )
  ftc_snode_weight( FTC_Node   snode,
                    FTC_Cache  cache );

  FT_LOCAL( FT_Bool )
  ftc_snode_compare( FTC_Node    snode,
                     FT_Pointer  gquery,
                     FTC_Cache   cache,
                     FT_Bool*    list_changed );


  FT_LOCAL( FT_Bool )
  ftc_gnode_compare( FTC_Node    gnode,
                     FT_Pointer  gquery,
                     FTC_Cache   cache,
                     FT_Bool*    list_changed );


  FT_LOCAL( FT_Error )
  ftc_gcache_init( FTC_Cache  cache );

  FT_LOCAL( void )
  ftc_gcache_done( FTC_Cache  cache );


  FT_LOCAL( FT_Error )
  ftc_cache_init( FTC_Cache  cache );

  FT_LOCAL( void )
  ftc_cache_done( FTC_Cache  cache );

  FT_LOCAL( void )
  ftc_node_destroy( FTC_Node     node,
                    FTC_Manager  manager );


#endif /* __FTCCBACK_H__ */

/* END */
