/////////////////////////////////////////////////////////////////////////////
// Name:        wx/icon.h
// Purpose:     wxIcon base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: icon.h 41538 2006-09-30 20:45:15Z RR $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ICON_H_BASE_
#define _WX_ICON_H_BASE_

#include "wx_iconloc.h"

#if defined(__WXPALMOS__)
  #include "wxMono_generic_icon.h"
#elif defined(__WXMSW__)
  #include "wxMono_msw_icon.h"
#elif defined(__WXMOTIF__)
  #include "wxMono_motif_icon.h"
#elif defined(__WXGTK20__)
  #include "wxMono_generic_icon.h"
#elif defined(__WXGTK__)
  #include "wxMono_generic_icon.h"
#elif defined(__WXX11__)
  #include "wxMono_generic_icon.h"
#elif defined(__WXMGL__)
  #define wxICON_DEFAULT_BITMAP_TYPE wxBITMAP_TYPE_ICO_RESOURCE
  #include "wxMono_generic_icon.h"
#elif defined(__WXDFB__)
  #include "wxMono_generic_icon.h"
#elif defined(__WXMAC__)
  #include "wxMono_mac_icon.h"
#elif defined(__WXCOCOA__)
  #include "wxMono_cocoa_icon.h"
#elif defined(__WXPM__)
  #include "wxMono_os2_icon.h"
#endif

//-----------------------------------------------------------------------------
// wxVariant support
//-----------------------------------------------------------------------------

#if wxUSE_VARIANT
#include "wx_variant.h"
DECLARE_VARIANT_OBJECT_EXPORTED(wxIcon,WXDLLEXPORT)
#endif


#endif
    // _WX_ICON_H_BASE_
