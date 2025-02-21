/////////////////////////////////////////////////////////////////////////////
// Name:        joystick.h
// Purpose:     wxJoystick base header
// Author:      wxWidgets Team
// Modified by:
// Created:
// Copyright:   (c) wxWidgets Team
// RCS-ID:      $Id: joystick.h 32852 2005-03-16 16:18:31Z ABX $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_JOYSTICK_H_BASE_
#define _WX_JOYSTICK_H_BASE_

#include "wx_defs.h"

#if wxUSE_JOYSTICK

#if defined(__WXMSW__)
#include "wxMono_msw_joystick.h"
#elif defined(__WXMOTIF__)
#include "wxMono_unix_joystick.h"
#elif defined(__WXGTK__)
#include "wxMono_unix_joystick.h"
#elif defined(__WXX11__)
#include "wxMono_unix_joystick.h"
#elif defined(__DARWIN__)
#include "wxMono_mac_joystick.h"
#elif defined(__WXMAC__)
#include "wxMono_mac_joystick.h"
#elif defined(__WXPM__)
#include "wxMono_os2_joystick.h"
#endif

#endif // wxUSE_JOYSTICK

#endif
    // _WX_JOYSTICK_H_BASE_
