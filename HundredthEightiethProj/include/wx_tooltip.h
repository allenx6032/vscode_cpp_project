/////////////////////////////////////////////////////////////////////////////
// Name:        tooltip.h
// Purpose:     wxToolTip base header
// Author:      Robert Roebling
// Modified by:
// Created:
// Copyright:   (c) Robert Roebling
// RCS-ID:      $Id: tooltip.h 37066 2006-01-23 03:27:34Z MR $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLTIP_H_BASE_
#define _WX_TOOLTIP_H_BASE_

#include "wx_defs.h"

#if wxUSE_TOOLTIPS

#if defined(__WXMSW__)
#include "wxMono_msw_tooltip.h"
#elif defined(__WXMOTIF__)
// #include "wxMono_motif_tooltip.h"
#elif defined(__WXGTK20__)
#include "wxMono_gtk_tooltip.h"
#elif defined(__WXGTK__)
#include "wxMono_gtk1_tooltip.h"
#elif defined(__WXMAC__)
#include "wxMono_mac_tooltip.h"
#elif defined(__WXCOCOA__)
#include "wxMono_cocoa_tooltip.h"
#elif defined(__WXPM__)
#include "wxMono_os2_tooltip.h"
#endif

#endif
    // wxUSE_TOOLTIPS

#endif
    // _WX_TOOLTIP_H_BASE_
