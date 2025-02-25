/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dcscreen.cpp
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: dcscreen.cpp 39123 2006-05-09 13:55:29Z ABX $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx_wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx_dcscreen.h"

#ifndef WX_PRECOMP
   #include "wx_string.h"
   #include "wx_window.h"
#endif

#include "wxMono_msw_private.h"

IMPLEMENT_DYNAMIC_CLASS(wxScreenDC, wxDC)

// Create a DC representing the whole screen
wxScreenDC::wxScreenDC()
{
    m_hDC = (WXHDC) ::GetDC((HWND) NULL);

    // the background mode is only used for text background and is set in
    // DrawText() to OPAQUE as required, otherwise always TRANSPARENT
    ::SetBkMode( GetHdc(), TRANSPARENT );
}
