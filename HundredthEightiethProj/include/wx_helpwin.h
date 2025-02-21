/////////////////////////////////////////////////////////////////////////////
// Name:        helpwin.h
// Purpose:     Includes Windows or OS/2 help
// Author:      Julian Smart
// Modified by:
// Created:
// RCS-ID:      $Id: helpwin.h 33948 2005-05-04 18:57:50Z JS $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPWIN_H_BASE_
#define _WX_HELPWIN_H_BASE_

#if defined(__WXMSW__)
#include "wxMono_msw_helpwin.h"
#elif defined(__WXPM__)
#include "wxMono_os2_helpwin.h"
#endif

#endif
    // _WX_HELPWIN_H_BASE_
