/////////////////////////////////////////////////////////////////////////////
// Name:        wx/minifram.h
// Purpose:     wxMiniFrame base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: minifram.h 49299 2007-10-21 18:07:29Z PC $
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MINIFRAM_H_BASE_
#define _WX_MINIFRAM_H_BASE_

#include "wx_defs.h"

#if wxUSE_MINIFRAME

#if defined(__WXPALMOS__)
#include "wxMono_palmos_minifram.h"
#elif defined(__WXMSW__)
#include "wxMono_msw_minifram.h"
#elif defined(__WXMOTIF__)
#include "wxMono_motif_minifram.h"
#elif defined(__WXGTK20__)
#include "wxMono_gtk_minifram.h"
#elif defined(__WXGTK__)
#include "wxMono_gtk1_minifram.h"
#elif defined(__WXX11__)
#include "wxMono_x11_minifram.h"
#elif defined(__WXMAC__)
#include "wxMono_mac_minifram.h"
#elif defined(__WXPM__)
#include "wxMono_os2_minifram.h"
#else
// TODO: it seems that wxMiniFrame could be just defined here generically
//       instead of having all the above port-specific headers
#include "wx_frame.h"
typedef wxFrame wxMiniFrame;
#endif

#endif // wxUSE_MINIFRAME
#endif // _WX_MINIFRAM_H_BASE_
