///////////////////////////////////////////////////////////////////////////////
// Name:        wx/checklst.h
// Purpose:     wxCheckListBox class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.00
// RCS-ID:      $Id: checklst.h 38319 2006-03-23 22:05:23Z VZ $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKLST_H_BASE_
#define _WX_CHECKLST_H_BASE_

#if wxUSE_CHECKLISTBOX

#include "wx_listbox.h"

// ----------------------------------------------------------------------------
// wxCheckListBox: a listbox whose items may be checked
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCheckListBoxBase : public
                                              #ifdef __WXWINCE__
                                                  // keep virtuals synchronised
                                                  wxListBoxBase
                                              #else
                                                  wxListBox
                                              #endif
{
public:
    wxCheckListBoxBase() { }

    // check list box specific methods
    virtual bool IsChecked(unsigned int item) const = 0;
    virtual void Check(unsigned int item, bool check = true) = 0;

    DECLARE_NO_COPY_CLASS(wxCheckListBoxBase)
};

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_checklst.h"
#elif defined(__WXWINCE__)
    #include "wxMono_msw_checklst.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_checklst.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_checklst.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_checklst.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_checklst.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_checklst.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_checklst.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_checklst.h"
#endif

#endif // wxUSE_CHECKLISTBOX

#endif
    // _WX_CHECKLST_H_BASE_
