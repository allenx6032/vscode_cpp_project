/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tabctrl.h
// Purpose:     wxTabCtrl base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: tabctrl.h 38943 2006-04-28 10:14:27Z ABX $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TABCTRL_H_BASE_
#define _WX_TABCTRL_H_BASE_

#include "wx_defs.h"

#if wxUSE_TAB_DIALOG

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGED, 800)
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_TAB_SEL_CHANGING, 801)
END_DECLARE_EVENT_TYPES()

#if defined(__WXMSW__)
    #include "wxMono_msw_tabctrl.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_tabctrl.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_tabctrl.h"
#endif

#endif // wxUSE_TAB_DIALOG
#endif
    // _WX_TABCTRL_H_BASE_
