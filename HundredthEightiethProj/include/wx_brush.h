/////////////////////////////////////////////////////////////////////////////
// Name:        wx/brush.h
// Purpose:     Includes platform-specific wxBrush file
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id: brush.h 40865 2006-08-27 09:42:42Z VS $
// Copyright:   Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BRUSH_H_BASE_
#define _WX_BRUSH_H_BASE_

#include "wx_defs.h"
#include "wx_object.h"
#include "wx_gdiobj.h"

// wxBrushBase
class WXDLLEXPORT wxBrushBase: public wxGDIObject
{
public:
    virtual ~wxBrushBase() { }

    virtual int GetStyle() const = 0;

    virtual bool IsHatch() const
        { return (GetStyle()>=wxFIRST_HATCH) && (GetStyle()<=wxLAST_HATCH); }
};

#if defined(__WXPALMOS__)
    #include "wxMono_palmos_brush.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_brush.h"
#elif defined(__WXMOTIF__) || defined(__WXX11__)
    #include "wxMono_x11_brush.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_brush.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_brush.h"
#elif defined(__WXMGL__)
    #include "wxMono_mgl_brush.h"
#elif defined(__WXDFB__)
    #include "wxMono_dfb_brush.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_brush.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_brush.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_brush.h"
#endif

#endif
    // _WX_BRUSH_H_BASE_
