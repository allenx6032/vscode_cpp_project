/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msgdlgg.h
// Purpose:     common header and base class for wxMessageDialog
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id: msgdlg.h 61872 2009-09-09 22:37:05Z VZ $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSGDLG_H_BASE_
#define _WX_MSGDLG_H_BASE_

#include "wx_defs.h"

#if wxUSE_MSGDLG

class WXDLLEXPORT wxMessageDialogBase
{
protected:
    // common validation of wxMessageDialog style
    void SetMessageDialogStyle(long style)
    {
        wxASSERT_MSG( ((style & wxYES_NO) == wxYES_NO) || ((style & wxYES_NO) == 0),
                      wxT("wxYES and wxNO may only be used together in wxMessageDialog") );

        wxASSERT_MSG( (style & wxID_OK) != wxID_OK,
                      wxT("wxMessageBox: Did you mean wxOK (and not wxID_OK)?") );

        m_dialogStyle = style;
    }
    inline long GetMessageDialogStyle() const
    {
        return m_dialogStyle;
    }

private:
    long m_dialogStyle;
};

#if defined(__WX_COMPILING_MSGDLGG_CPP__)
#include "wxMono_generic_msgdlgg.h"
#elif defined(__WXUNIVERSAL__) || defined(__WXGPE__)
#include "wxMono_generic_msgdlgg.h"
#elif defined(__WXPALMOS__)
#include "wxMono_palmos_msgdlg.h"
#elif defined(__WXMSW__)
#include "wxMono_msw_msgdlg.h"
#elif defined(__WXMOTIF__)
#include "wxMono_motif_msgdlg.h"
#elif defined(__WXGTK20__)
#include "wxMono_gtk_msgdlg.h"
#elif defined(__WXGTK__)
#include "wxMono_generic_msgdlgg.h"
#elif defined(__WXGTK__)
#include "wxMono_generic_msgdlgg.h"
#elif defined(__WXMAC__)
#include "wxMono_mac_msgdlg.h"
#elif defined(__WXCOCOA__)
#include "wxMono_cocoa_msgdlg.h"
#elif defined(__WXPM__)
#include "wxMono_os2_msgdlg.h"
#endif

// ----------------------------------------------------------------------------
// wxMessageBox: the simplest way to use wxMessageDialog
// ----------------------------------------------------------------------------

int WXDLLEXPORT wxMessageBox(const wxString& message,
                             const wxString& caption = wxMessageBoxCaptionStr,
                             long style = wxOK | wxCENTRE,
                             wxWindow *parent = NULL,
                             int x = wxDefaultCoord, int y = wxDefaultCoord);

#endif // wxUSE_MSGDLG

#endif
    // _WX_MSGDLG_H_BASE_
