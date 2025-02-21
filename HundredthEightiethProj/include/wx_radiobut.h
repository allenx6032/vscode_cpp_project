///////////////////////////////////////////////////////////////////////////////
// Name:        wx/radiobut.h
// Purpose:     wxRadioButton declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.09.00
// RCS-ID:      $Id: radiobut.h 37066 2006-01-23 03:27:34Z MR $
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_BASE_
#define _WX_RADIOBUT_H_BASE_

#include "wx_defs.h"

#if wxUSE_RADIOBTN

/*
   There is no wxRadioButtonBase class as wxRadioButton interface is the same
   as wxCheckBox(Base), but under some platforms wxRadioButton really
   derives from wxCheckBox and on the others it doesn't.

   The pseudo-declaration of wxRadioButtonBase would look like this:

   class wxRadioButtonBase : public ...
   {
   public:
        virtual void SetValue(bool value);
        virtual bool GetValue() const;
   };
 */

#include "wx_control.h"

extern WXDLLEXPORT_DATA(const wxChar) wxRadioButtonNameStr[];

#if defined(__WXUNIVERSAL__)
    #include "wxMono_univ_radiobut.h"
#elif defined(__WXMSW__)
    #include "wxMono_msw_radiobut.h"
#elif defined(__WXMOTIF__)
    #include "wxMono_motif_radiobut.h"
#elif defined(__WXGTK20__)
    #include "wxMono_gtk_radiobut.h"
#elif defined(__WXGTK__)
    #include "wxMono_gtk1_radiobut.h"
#elif defined(__WXMAC__)
    #include "wxMono_mac_radiobut.h"
#elif defined(__WXCOCOA__)
    #include "wxMono_cocoa_radiobut.h"
#elif defined(__WXPM__)
    #include "wxMono_os2_radiobut.h"
#elif defined(__WXPALMOS__)
    #include "wxMono_palmos_radiobut.h"
#endif

#endif // wxUSE_RADIOBTN

#endif
    // _WX_RADIOBUT_H_BASE_
