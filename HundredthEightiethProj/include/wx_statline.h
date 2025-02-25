/////////////////////////////////////////////////////////////////////////////
// Name:        wx/statline.h
// Purpose:     wxStaticLine class interface
// Author:      Vadim Zeitlin
// Created:     28.06.99
// Version:     $Id: statline.h 43874 2006-12-09 14:52:59Z VZ $
// Copyright:   (c) 1999 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATLINE_H_BASE_
#define _WX_STATLINE_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// this defines wxUSE_STATLINE
#include "wx_defs.h"

#if wxUSE_STATLINE

// the base class declaration
#include "wx_control.h"

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// the default name for objects of class wxStaticLine
extern WXDLLEXPORT_DATA(const wxChar) wxStaticLineNameStr[];

// ----------------------------------------------------------------------------
// wxStaticLine - a line in a dialog
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStaticLineBase : public wxControl
{
public:
    // constructor
    wxStaticLineBase() { }

    // is the line vertical?
    bool IsVertical() const { return (GetWindowStyle() & wxLI_VERTICAL) != 0; }

    // get the default size for the "lesser" dimension of the static line
    static int GetDefaultSize() { return 2; }

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return false; }

protected:
    // set the right size for the right dimension
    wxSize AdjustSize(const wxSize& size) const
    {
        wxSize sizeReal(size);
        if ( IsVertical() )
        {
            if ( size.x == wxDefaultCoord )
                sizeReal.x = GetDefaultSize();
        }
        else
        {
            if ( size.y == wxDefaultCoord )
                sizeReal.y = GetDefaultSize();
        }

        return sizeReal;
    }

    virtual wxSize DoGetBestSize() const
    {
        return AdjustSize(wxDefaultSize);
    }

    DECLARE_NO_COPY_CLASS(wxStaticLineBase)
};

// ----------------------------------------------------------------------------
// now include the actual class declaration
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_statline.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_statline.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_statline.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_statline.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_statline.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_statline.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_statline.h"
#else // use generic implementation for all other platforms
    #include "wxMono_generic_statline.h"
#endif

#endif // wxUSE_STATLINE

#endif // _WX_STATLINE_H_BASE_
