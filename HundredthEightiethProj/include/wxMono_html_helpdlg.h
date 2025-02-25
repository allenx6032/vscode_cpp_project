/////////////////////////////////////////////////////////////////////////////
// Name:        wx/html/helpdlg.h
// Purpose:     wxHtmlHelpDialog
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden, Vaclav Slavik, Julian Smart
// RCS-ID:      $Id: helpdlg.h 49804 2007-11-10 01:09:42Z VZ $
// Copyright:   (c) Harm van der Heijden, Vaclav Slavik, Julian Smart
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPDLG_H_
#define _WX_HELPDLG_H_

#include "wx_defs.h"

#if wxUSE_WXHTML_HELP

#include "wxMono_html_helpdata.h"
#include "wx_window.h"
#include "wx_dialog.h"
#include "wx_frame.h"
#include "wx_config.h"
#include "wx_splitter.h"
#include "wx_notebook.h"
#include "wx_listbox.h"
#include "wx_choice.h"
#include "wx_combobox.h"
#include "wx_checkbox.h"
#include "wx_stattext.h"
#include "wxMono_html_htmlwin.h"
#include "wxMono_html_helpwnd.h"
#include "wxMono_html_htmprint.h"

class WXDLLIMPEXP_FWD_HTML wxHtmlHelpController;
class WXDLLIMPEXP_FWD_HTML wxHtmlHelpWindow;

class WXDLLIMPEXP_HTML wxHtmlHelpDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(wxHtmlHelpDialog)

public:
    wxHtmlHelpDialog(wxHtmlHelpData* data = NULL) { Init(data); }
    wxHtmlHelpDialog(wxWindow* parent, wxWindowID wxWindowID,
                    const wxString& title = wxEmptyString,
                    int style = wxHF_DEFAULT_STYLE, wxHtmlHelpData* data = NULL);
    virtual ~wxHtmlHelpDialog();

    bool Create(wxWindow* parent, wxWindowID id, const wxString& title = wxEmptyString,
                int style = wxHF_DEFAULT_STYLE);

    /// Returns the data associated with this dialog.
    wxHtmlHelpData* GetData() { return m_Data; }

    /// Returns the controller that created this dialog.
    wxHtmlHelpController* GetController() const { return m_helpController; }

    /// Sets the controller associated with this dialog.
    void SetController(wxHtmlHelpController* controller) { m_helpController = controller; }

    /// Returns the help window.
    wxHtmlHelpWindow* GetHelpWindow() const { return m_HtmlHelpWin; }

    // Sets format of title of the frame. Must contain exactly one "%s"
    // (for title of displayed HTML page)
    void SetTitleFormat(const wxString& format);

    // Override to add custom buttons to the toolbar
    virtual void AddToolbarButtons(wxToolBar* WXUNUSED(toolBar), int WXUNUSED(style)) {}

protected:
    void Init(wxHtmlHelpData* data = NULL);

    void OnCloseWindow(wxCloseEvent& event);

protected:
    // Temporary pointer to pass to window
    wxHtmlHelpData* m_Data;
    wxString m_TitleFormat;  // title of the help frame
    wxHtmlHelpWindow *m_HtmlHelpWin;
    wxHtmlHelpController* m_helpController;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxHtmlHelpDialog)
};

#endif
    // wxUSE_WXHTML_HELP

#endif
