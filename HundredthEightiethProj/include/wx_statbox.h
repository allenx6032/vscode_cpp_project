/////////////////////////////////////////////////////////////////////////////
// Name:        statbox.h
// Purpose:     wxStaticBox base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: statbox.h 37066 2006-01-23 03:27:34Z MR $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBOX_H_BASE_
#define _WX_STATBOX_H_BASE_

#include "wx_defs.h"

#if wxUSE_STATBOX

#include "wx_control.h"

extern WXDLLEXPORT_DATA(const wxChar) wxStaticBoxNameStr[];

// ----------------------------------------------------------------------------
// wxStaticBox: a grouping box with a label
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStaticBoxBase : public wxControl
{
public:
    wxStaticBoxBase() { }

    // overriden base class virtuals
    virtual bool AcceptsFocus() const { return false; }
    virtual bool HasTransparentBackground() { return true; }

    // implementation only: this is used by wxStaticBoxSizer to account for the
    // need for extra space taken by the static box
    //
    // the top border is the margin at the top (where the title is),
    // borderOther is the margin on all other sides
    virtual void GetBordersForSizer(int *borderTop, int *borderOther) const
    {
        const int BORDER = 5; // FIXME: hardcoded value

        *borderTop = GetLabel().empty() ? BORDER : GetCharHeight();
        *borderOther = BORDER;
    }

private:
    DECLARE_NO_COPY_CLASS(wxStaticBoxBase)
};

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_statbox.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_statbox.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_statbox.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_statbox.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_statbox.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_statbox.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_statbox.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_statbox.h"
#endif

#endif // wxUSE_STATBOX

#endif
    // _WX_STATBOX_H_BASE_
