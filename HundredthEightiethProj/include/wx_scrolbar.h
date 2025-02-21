/////////////////////////////////////////////////////////////////////////////
// Name:        scrolbar.h
// Purpose:     wxScrollBar base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: scrolbar.h 37066 2006-01-23 03:27:34Z MR $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SCROLBAR_H_BASE_
#define _WX_SCROLBAR_H_BASE_

#include "wx_defs.h"

#if wxUSE_SCROLLBAR

#include "wx_control.h"

extern WXDLLEXPORT_DATA(const wxChar) wxScrollBarNameStr[];

// ----------------------------------------------------------------------------
// wxScrollBar: a scroll bar control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxScrollBarBase : public wxControl
{
public:
    wxScrollBarBase() { }

    // scrollbar construction
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSB_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxScrollBarNameStr);

    // accessors
    virtual int GetThumbPosition() const = 0;
    virtual int GetThumbSize() const = 0;
    virtual int GetPageSize() const = 0;
    virtual int GetRange() const = 0;

    bool IsVertical() const { return (m_windowStyle & wxVERTICAL) != 0; }

    // operations
    virtual void SetThumbPosition(int viewStart) = 0;
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = true) = 0;

private:
    DECLARE_NO_COPY_CLASS(wxScrollBarBase)
};

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_scrolbar.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_scrolbar.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_scrolbar.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_scrolbar.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_scrolbar.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_scrolbar.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_scrolbar.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_scrolbar.h"
#endif

#endif // wxUSE_SCROLLBAR

#endif
    // _WX_SCROLBAR_H_BASE_
