/////////////////////////////////////////////////////////////////////////////
// Name:        config.h
// Purpose:     wxConfig base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: config.h 60524 2009-05-05 22:51:44Z PC $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONFIG_H_BASE_
#define _WX_CONFIG_H_BASE_

#include "wx_confbase.h"

#if wxUSE_CONFIG

#if defined(__WXMSW__) && wxUSE_CONFIG_NATIVE
#    ifdef __WIN32__
#        include "wxMono_msw_regconf.h"
#    else
#        include "wxMono_msw_iniconf.h"
#    endif
#elif defined(__WXPALMOS__) && wxUSE_CONFIG_NATIVE
#    include "wxMono_palmos_prefconf.h"
#else
#    include "wx_fileconf.h"
#endif

#endif // wxUSE_CONFIG

#endif // _WX_CONFIG_H_BASE_
