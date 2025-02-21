/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mdi.h
// Purpose:     wxMDI base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: mdi.h 37066 2006-01-23 03:27:34Z MR $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MDI_H_BASE_
#define _WX_MDI_H_BASE_

#include "wx_defs.h"

#if wxUSE_MDI

#if defined(__WXUNIVERSAL__)
    #include "wxMono_generic_mdig.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_mdi.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_mdi.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_mdi.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_mdi.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_mdi.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_mdi.h"
#elif defined(__WXPM__)
    #include "wxMono_generic_mdig.h"
#endif

#endif // wxUSE_MDI

#endif
    // _WX_MDI_H_BASE_
