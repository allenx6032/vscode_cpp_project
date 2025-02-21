/////////////////////////////////////////////////////////////////////////////
// Name:        help.h
// Purpose:     wxHelpController base header
// Author:      wxWidgets Team
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: help.h 33948 2005-05-04 18:57:50Z JS $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELP_H_BASE_
#define _WX_HELP_H_BASE_

#include "wx_defs.h"

#if wxUSE_HELP

#include "wx_helpbase.h"

#ifdef __WXWINCE__
    #include "wxMono_msw_helpwce.h"

    #define wxHelpController wxWinceHelpController
#elif defined(__WXPALMOS__)
    #include "wxMono_palmos_help.h"

    #define wxHelpController wxPalmHelpController
#elif defined(__WXMSW__)
    #include "wxMono_msw_helpchm.h"

    #define wxHelpController wxCHMHelpController
#else // !MSW

#if wxUSE_WXHTML_HELP
    #include "wxMono_html_helpctrl.h"
    #define wxHelpController wxHtmlHelpController
#else
    #include "wxMono_generic_helpext.h"
    #define wxHelpController wxExtHelpController
#endif

#endif // MSW/!MSW

#endif // wxUSE_HELP

#endif
    // _WX_HELP_H_BASE_
