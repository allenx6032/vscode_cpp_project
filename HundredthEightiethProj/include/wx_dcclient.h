/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcclient.h
// Purpose:     wxClientDC base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: dcclient.h 40865 2006-08-27 09:42:42Z VS $
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_BASE_
#define _WX_DCCLIENT_H_BASE_

#include "wx_defs.h"

#if defined(__WXPALMOS__)
#include "wxMono_palmos_dcclient.h"
#elif defined(__WXMSW__)
#include "wxMono_msw_dcclient.h"
#elif defined(__WXMOTIF__)
#include "wxMono_motif_dcclient.h"
#elif defined(__WXGTK20__)
#include "wxMono_gtk_dcclient.h"
#elif defined(__WXGTK__)
#include "wxMono_gtk1_dcclient.h"
#elif defined(__WXX11__)
#include "wxMono_x11_dcclient.h"
#elif defined(__WXMGL__)
#include "wxMono_mgl_dcclient.h"
#elif defined(__WXDFB__)
#include "wxMono_dfb_dcclient.h"
#elif defined(__WXMAC__)
#include "wxMono_mac_dcclient.h"
#elif defined(__WXCOCOA__)
#include "wxMono_cocoa_dcclient.h"
#elif defined(__WXPM__)
#include "wxMono_os2_dcclient.h"
#endif

#endif
    // _WX_DCCLIENT_H_BASE_
