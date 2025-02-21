/////////////////////////////////////////////////////////////////////////////
// Name:        wx/choice.h
// Purpose:     wxChoice class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     26.07.99
// RCS-ID:      $Id: choice.h 42727 2006-10-30 16:04:27Z VZ $
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHOICE_H_BASE_
#define _WX_CHOICE_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx_defs.h"

#if wxUSE_CHOICE

#include "wx_ctrlsub.h"     // the base class

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

extern WXDLLEXPORT_DATA(const wxChar) wxChoiceNameStr[];

// ----------------------------------------------------------------------------
// wxChoice allows to select one of a non-modifiable list of strings
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxChoiceBase : public wxControlWithItems
{
public:
    wxChoiceBase() { }
    virtual ~wxChoiceBase();

    // all generic methods are in wxControlWithItems

    // get the current selection: this can only be different from the normal
    // selection if the popup items list is currently opened and the user
    // selected some item in it but didn't close the list yet; otherwise (and
    // currently always on platforms other than MSW) this is the same as
    // GetSelection()
    virtual int GetCurrentSelection() const { return GetSelection(); }

    // set/get the number of columns in the control (as they're not supported on
    // most platforms, they do nothing by default)
    virtual void SetColumns(int WXUNUSED(n) = 1 ) { }
    virtual int GetColumns() const { return 1 ; }

    // emulate selecting the item event.GetInt()
    void Command(wxCommandEvent& event);

private:
    DECLARE_NO_COPY_CLASS(wxChoiceBase)
};

// ----------------------------------------------------------------------------
// include the platform-dependent class definition
// ----------------------------------------------------------------------------

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_choice.h"
#elif defined(__SMARTPHONE__) && defined(__WXWINCE__)
    #include "wxMono_msw_choicece.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_choice.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_choice.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_choice.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_choice.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_choice.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_choice.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_choice.h"
#endif

#endif // wxUSE_CHOICE

#endif // _WX_CHOICE_H_BASE_
