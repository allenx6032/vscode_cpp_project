/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wx.h
// Purpose:     wxWidgets central header including the most often used ones
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id: wx.h 40943 2006-08-31 19:31:43Z ABX $
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WX_H_
#define _WX_WX_H_

#include "wx_defs.h"
#include "wx_object.h"
#include "wx_dynarray.h"
#include "wx_list.h"
#include "wx_hash.h"
#include "wx_string.h"
#include "wx_hashmap.h"
#include "wx_arrstr.h"
#include "wx_intl.h"
#include "wx_log.h"
#include "wx_event.h"
#include "wx_app.h"
#include "wx_utils.h"
#include "wx_stream.h"
#include "wx_memory.h"
#include "wx_math.h"
#include "wx_stopwatch.h"
#include "wx_module.h"

#if wxUSE_GUI

#include "wx_window.h"
#include "wx_containr.h"
#include "wx_panel.h"
#include "wx_toplevel.h"
#include "wx_frame.h"
#include "wx_gdicmn.h"
#include "wx_gdiobj.h"
#include "wx_region.h"
#include "wx_bitmap.h"
#include "wx_image.h"
#include "wx_colour.h"
#include "wx_font.h"
#include "wx_dc.h"
#include "wx_dcclient.h"
#include "wx_dcmemory.h"
#include "wx_dcprint.h"
#include "wx_dcscreen.h"
#include "wx_button.h"
#include "wx_menuitem.h"
#include "wx_menu.h"
#include "wx_pen.h"
#include "wx_brush.h"
#include "wx_palette.h"
#include "wx_icon.h"
#include "wx_cursor.h"
#include "wx_dialog.h"
#include "wx_timer.h"
#include "wx_settings.h"
#include "wx_msgdlg.h"
#include "wx_cmndata.h"
#include "wx_dataobj.h"

#include "wx_control.h"
#include "wx_ctrlsub.h"
#include "wx_bmpbuttn.h"
#include "wx_checkbox.h"
#include "wx_checklst.h"
#include "wx_choice.h"
#include "wx_scrolbar.h"
#include "wx_stattext.h"
#include "wx_statbmp.h"
#include "wx_statbox.h"
#include "wx_listbox.h"
#include "wx_radiobox.h"
#include "wx_radiobut.h"
#include "wx_textctrl.h"
#include "wx_slider.h"
#include "wx_gauge.h"
#include "wx_scrolwin.h"
#include "wx_dirdlg.h"
#include "wx_toolbar.h"
#include "wx_combobox.h"
#include "wx_layout.h"
#include "wx_sizer.h"
#include "wx_mdi.h"
#include "wx_statusbr.h"
#include "wx_choicdlg.h"
#include "wx_textdlg.h"
#include "wx_filedlg.h"

// always include, even if !wxUSE_VALIDATORS because we need wxDefaultValidator
#include "wx_validate.h"

#if wxUSE_VALIDATORS
    #include "wx_valtext.h"
#endif // wxUSE_VALIDATORS

#endif // wxUSE_GUI

#endif // _WX_WX_H_
