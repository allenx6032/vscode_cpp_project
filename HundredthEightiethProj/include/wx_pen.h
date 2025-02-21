/////////////////////////////////////////////////////////////////////////////
// Name:        wx/pen.h
// Purpose:     Base header for wxPen
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id: pen.h 40865 2006-08-27 09:42:42Z VS $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_BASE_
#define _WX_PEN_H_BASE_

#include "wx_defs.h"

#if defined(__WXPALMOS__)
#include "wxMono_palmos_pen.h"
#elif defined(__WXMSW__)
#include "wxMono_msw_pen.h"
#elif defined(__WXMOTIF__) || defined(__WXX11__)
#include "wxMono_x11_pen.h"
#elif defined(__WXGTK20__)
#include "wxMono_gtk_pen.h"
#elif defined(__WXGTK__)
#include "wxMono_gtk1_pen.h"
#elif defined(__WXMGL__)
#include "wxMono_mgl_pen.h"
#elif defined(__WXDFB__)
#include "wxMono_dfb_pen.h"
#elif defined(__WXMAC__)
#include "wxMono_mac_pen.h"
#elif defined(__WXCOCOA__)
#include "wxMono_cocoa_pen.h"
#elif defined(__WXPM__)
#include "wxMono_os2_pen.h"
#endif

#endif
    // _WX_PEN_H_BASE_
