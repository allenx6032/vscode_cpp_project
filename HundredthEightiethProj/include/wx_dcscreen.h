/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcscreen.h
// Purpose:     wxScreenDC base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: dcscreen.h 40865 2006-08-27 09:42:42Z VS $
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCSCREEN_H_BASE_
#define _WX_DCSCREEN_H_BASE_

#include "wx_defs.h"

#if defined(__WXPALMOS__)
#include "wxMono_palmos_dcscreen.h"
#elif defined(__WXMSW__)
#include "wxMono_msw_dcscreen.h"
#elif defined(__WXMOTIF__)
#include "wxMono_motif_dcscreen.h"
#elif defined(__WXGTK20__)
#include "wxMono_gtk_dcscreen.h"
#elif defined(__WXGTK__)
#include "wxMono_gtk1_dcscreen.h"
#elif defined(__WXX11__)
#include "wxMono_x11_dcscreen.h"
#elif defined(__WXMGL__)
#include "wxMono_mgl_dcscreen.h"
#elif defined(__WXDFB__)
#include "wxMono_dfb_dcscreen.h"
#elif defined(__WXMAC__)
#include "wxMono_mac_dcscreen.h"
#elif defined(__WXCOCOA__)
#include "wxMono_cocoa_dcscreen.h"
#elif defined(__WXPM__)
#include "wxMono_os2_dcscreen.h"
#endif

#endif
    // _WX_DCSCREEN_H_BASE_
