/////////////////////////////////////////////////////////////////////////////
// Name:        wx/button.h
// Purpose:     wxButtonBase class
// Author:      Vadim Zetlin
// Modified by:
// Created:     15.08.00
// RCS-ID:      $Id: button.h 49563 2007-10-31 20:46:21Z VZ $
// Copyright:   (c) Vadim Zetlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_BASE_
#define _WX_BUTTON_H_BASE_

#include "wx_defs.h"

// ----------------------------------------------------------------------------
// wxButton flags shared with other classes
// ----------------------------------------------------------------------------

#if wxUSE_TOGGLEBTN || wxUSE_BUTTON

// These flags affect label alignment
#define wxBU_LEFT            0x0040
#define wxBU_TOP             0x0080
#define wxBU_RIGHT           0x0100
#define wxBU_BOTTOM          0x0200
#define wxBU_ALIGN_MASK      ( wxBU_LEFT | wxBU_TOP | wxBU_RIGHT | wxBU_BOTTOM )
#endif

#if wxUSE_BUTTON

// ----------------------------------------------------------------------------
// wxButton specific flags
// ----------------------------------------------------------------------------

// These two flags are obsolete
#define wxBU_NOAUTODRAW      0x0000
#define wxBU_AUTODRAW        0x0004

// by default, the buttons will be created with some (system dependent)
// minimal size to make them look nicer, giving this style will make them as
// small as possible
#define wxBU_EXACTFIT        0x0001

#include "wx_control.h"

class WXDLLIMPEXP_FWD_CORE wxBitmap;

extern WXDLLEXPORT_DATA(const wxChar) wxButtonNameStr[];

// ----------------------------------------------------------------------------
// wxButton: a push button
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxButtonBase : public wxControl
{
public:
    wxButtonBase() { }

    // show the image in the button in addition to the label
    virtual void SetImageLabel(const wxBitmap& WXUNUSED(bitmap)) { }

    // set the margins around the image
    virtual void SetImageMargins(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y)) { }

    // this wxButton method is called when the button becomes the default one
    // on its panel
    virtual void SetDefault() { }

    // Buttons on MSW can look bad if they are not native colours, because
    // then they become owner-drawn and not theme-drawn.  Disable it here
    // in wxButtonBase to make it consistent.
    virtual bool ShouldInheritColours() const { return false; }

    // returns the default button size for this platform
    static wxSize GetDefaultSize();

protected:
    DECLARE_NO_COPY_CLASS(wxButtonBase)
};

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_button.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_button.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_button.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_button.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_button.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_button.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_button.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_button.h"
#elif defined(__WXPALMOS__)
    #include "wxMono_palmos_button.h"
#endif

#endif // wxUSE_BUTTON

#endif
    // _WX_BUTTON_H_BASE_
