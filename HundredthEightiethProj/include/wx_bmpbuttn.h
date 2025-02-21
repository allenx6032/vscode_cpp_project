/////////////////////////////////////////////////////////////////////////////
// Name:        wx/bmpbutton.h
// Purpose:     wxBitmapButton class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     25.08.00
// RCS-ID:      $Id: bmpbuttn.h 45498 2007-04-16 13:03:05Z VZ $
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BMPBUTTON_H_BASE_
#define _WX_BMPBUTTON_H_BASE_

#include "wx_defs.h"

#if wxUSE_BMPBUTTON

#include "wx_bitmap.h"
#include "wx_button.h"

extern WXDLLEXPORT_DATA(const wxChar) wxButtonNameStr[];

// ----------------------------------------------------------------------------
// wxBitmapButton: a button which shows bitmaps instead of the usual string.
// It has different bitmaps for different states (focused/disabled/pressed)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxBitmapButtonBase : public wxButton
{
public:
    wxBitmapButtonBase()
    {
        m_marginX =
        m_marginY = 0;
    }

    // set the bitmaps
    void SetBitmapLabel(const wxBitmap& bitmap)
        { m_bmpNormal = bitmap; OnSetBitmap(); }
    void SetBitmapSelected(const wxBitmap& sel)
        { m_bmpSelected = sel; OnSetBitmap(); }
    void SetBitmapFocus(const wxBitmap& focus)
        { m_bmpFocus = focus; OnSetBitmap(); }
    void SetBitmapDisabled(const wxBitmap& disabled)
        { m_bmpDisabled = disabled; OnSetBitmap(); }
    void SetBitmapHover(const wxBitmap& hover)
        { m_bmpHover = hover; OnSetBitmap(); }

    // retrieve the bitmaps
    const wxBitmap& GetBitmapLabel() const { return m_bmpNormal; }
    const wxBitmap& GetBitmapSelected() const { return m_bmpSelected; }
    const wxBitmap& GetBitmapFocus() const { return m_bmpFocus; }
    const wxBitmap& GetBitmapDisabled() const { return m_bmpDisabled; }
    const wxBitmap& GetBitmapHover() const { return m_bmpHover; }
    wxBitmap& GetBitmapLabel() { return m_bmpNormal; }
    wxBitmap& GetBitmapSelected() { return m_bmpSelected; }
    wxBitmap& GetBitmapFocus() { return m_bmpFocus; }
    wxBitmap& GetBitmapDisabled() { return m_bmpDisabled; }
    wxBitmap& GetBitmapHover() { return m_bmpHover; }

    // set/get the margins around the button
    virtual void SetMargins(int x, int y) { m_marginX = x; m_marginY = y; }
    int GetMarginX() const { return m_marginX; }
    int GetMarginY() const { return m_marginY; }

    // deprecated synonym for SetBitmapLabel()
#if WXWIN_COMPATIBILITY_2_6
    wxDEPRECATED( void SetLabel(const wxBitmap& bitmap) );

    // prevent virtual function hiding
    virtual void SetLabel(const wxString& label)
        { wxWindow::SetLabel(label); }
#endif // WXWIN_COMPATIBILITY_2_6

protected:
    // function called when any of the bitmaps changes
    virtual void OnSetBitmap() { InvalidateBestSize(); Refresh(); }

    // the bitmaps for various states
    wxBitmap m_bmpNormal,
             m_bmpSelected,
             m_bmpFocus,
             m_bmpDisabled,
             m_bmpHover;

    // the margins around the bitmap
    int m_marginX,
        m_marginY;


    DECLARE_NO_COPY_CLASS(wxBitmapButtonBase)
};

#if WXWIN_COMPATIBILITY_2_6
inline void wxBitmapButtonBase::SetLabel(const wxBitmap& bitmap)
{
    SetBitmapLabel(bitmap);
}
#endif // WXWIN_COMPATIBILITY_2_6

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_bmpbuttn.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_bmpbuttn.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_bmpbuttn.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_bmpbuttn.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_bmpbuttn.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_bmpbuttn.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_bmpbuttn.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_bmpbuttn.h"
#endif

#endif // wxUSE_BMPBUTTON

#endif // _WX_BMPBUTTON_H_BASE_
