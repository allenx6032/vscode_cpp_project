/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cursor.h
// Purpose:     wxCursor base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id: cursor.h 40865 2006-08-27 09:42:42Z VS $
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_BASE_
#define _WX_CURSOR_H_BASE_

#include "wx_defs.h"

#if defined(__WXPALMOS__)
    #include "wxMono_palmos_cursor.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_cursor.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_cursor.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_cursor.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_cursor.h"
#elif defined(__WXX11__)
    #include "wxMono_x11_cursor.h"
#elif defined(__WXMGL__)
    #include "wxMono_mgl_cursor.h"
#elif defined(__WXDFB__)
    #include "wxMono_dfb_cursor.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_cursor.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_cursor.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_cursor.h"
#endif

#include "wx_utils.h"

/* This is a small class which can be used by all ports
   to temporarily suspend the busy cursor. Useful in modal
   dialogs.

   Actually that is not (any longer) quite true..  currently it is
   only used in wxGTK Dialog::ShowModal() and now uses static
   wxBusyCursor methods that are only implemented for wxGTK so far.
   The BusyCursor handling code should probably be implemented in
   common code somewhere instead of the separate implementations we
   currently have.  Also the name BusyCursorSuspender is a little
   misleading since it doesn't actually suspend the BusyCursor, just
   masks one that is already showing.
   If another call to wxBeginBusyCursor is made while this is active
   the Busy Cursor will again be shown.  But at least now it doesn't
   interfere with the state of wxIsBusy() -- RL

*/
class wxBusyCursorSuspender
{
public:
    wxBusyCursorSuspender()
    {
        if( wxIsBusy() )
        {
            wxSetCursor( wxBusyCursor::GetStoredCursor() );
        }
    }
    ~wxBusyCursorSuspender()
    {
        if( wxIsBusy() )
        {
            wxSetCursor( wxBusyCursor::GetBusyCursor() );
        }
    }
};
#endif
    // _WX_CURSOR_H_BASE_
