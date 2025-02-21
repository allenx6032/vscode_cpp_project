/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:     wxStaticText base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: stattext.h 37066 2006-01-23 03:27:34Z MR $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_BASE_
#define _WX_STATTEXT_H_BASE_

#include "wx_defs.h"

#if wxUSE_STATTEXT

#include "wx_control.h"

extern WXDLLEXPORT_DATA(const wxChar) wxStaticTextNameStr[];

class WXDLLEXPORT wxStaticTextBase : public wxControl
{
public:
    wxStaticTextBase() { }

    // in wxGTK wxStaticText doesn't derive from wxStaticTextBase so we have to
    // declare this function directly in gtk header
#if !defined(__WXGTK__) || defined(__WXUNIVERSAL__)
    // wrap the text of the control so that no line is longer than the given
    // width (if possible: this function won't break words)
    //
    // NB: implemented in dlgcmn.cpp for now
    void Wrap(int width);
#endif // ! native __WXGTK__

    // overriden base virtuals
    virtual bool AcceptsFocus() const { return false; }
    virtual bool HasTransparentBackground() { return true; }

private:
    DECLARE_NO_COPY_CLASS(wxStaticTextBase)
};

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_stattext.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_stattext.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_stattext.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_stattext.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_stattext.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_stattext.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_stattext.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_stattext.h"
#elif defined(__WXPALMOS__)
    #include "wxMono_palmos_stattext.h"
#endif

#endif // wxUSE_STATTEXT

#endif
    // _WX_STATTEXT_H_BASE_
