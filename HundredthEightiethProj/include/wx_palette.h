/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palette.h
// Purpose:     Common header and base class for wxPalette
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id: palette.h 61872 2009-09-09 22:37:05Z VZ $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PALETTE_H_BASE_
#define _WX_PALETTE_H_BASE_

#include "wx_defs.h"

#if wxUSE_PALETTE

#include "wx_object.h"
#include "wx_gdiobj.h"

// wxPaletteBase
class WXDLLEXPORT wxPaletteBase: public wxGDIObject
{
public:
    virtual ~wxPaletteBase() { }

    virtual bool Ok() const { return IsOk(); }
    virtual bool IsOk() const = 0;
    virtual int GetColoursCount() const { wxFAIL_MSG( wxT("not implemented") ); return 0; }
};

#if defined(__WXPALMOS__)
    #include "wxMono_palmos_palette.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_palette.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_palette.h"
#elif defined(__WXGTK__) || defined(__WXCOCOA__)
    #include "wxMono_generic_paletteg.h"
#elif defined(__WXX11__)
    #include "wxMono_x11_palette.h"
#elif defined(__WXMGL__)
    #include "wxMono_mgl_palette.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_palette.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_palette.h"
#endif

#if WXWIN_COMPATIBILITY_2_4
    #define wxColorMap wxPalette
    #define wxColourMap wxPalette
#endif

#endif // wxUSE_PALETTE

#endif
    // _WX_PALETTE_H_BASE_
