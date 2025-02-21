/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/artstd.cpp
// Purpose:     stock wxArtProvider instance with default wxWin art
// Author:      Vaclav Slavik
// Modified by:
// Created:     18/03/2002
// RCS-ID:      $Id: artstd.cpp 52561 2008-03-16 00:36:37Z VS $
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx_wxprec.h"

#if defined(__BORLANDC__)
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx_image.h"
#endif

#include "wx_artprov.h"

// ----------------------------------------------------------------------------
// wxDefaultArtProvider
// ----------------------------------------------------------------------------

class wxDefaultArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
};

// ----------------------------------------------------------------------------
// helper macros
// ----------------------------------------------------------------------------

// Standard macro for getting a resource from XPM file:
#define ART(artId, xpmRc) \
    if ( id == artId ) return wxBitmap(xpmRc##_xpm);

// There are two ways of getting the standard icon: either via XPMs or via
// wxIcon ctor. This depends on the platform:
#if defined(__WXUNIVERSAL__)
    #define CREATE_STD_ICON(iconId, xpmRc) return wxNullBitmap;
#elif defined(__WXGTK__) || defined(__WXMOTIF__)
    #define CREATE_STD_ICON(iconId, xpmRc) return wxBitmap(xpmRc##_xpm);
#else
    #define CREATE_STD_ICON(iconId, xpmRc) \
        { \
            wxIcon icon(_T(iconId)); \
            wxBitmap bmp; \
            bmp.CopyFromIcon(icon); \
            return bmp; \
        }
#endif

// Macro used in CreateBitmap to get wxICON_FOO icons:
#define ART_MSGBOX(artId, iconId, xpmRc) \
    if ( id == artId ) \
    { \
        CREATE_STD_ICON(#iconId, xpmRc) \
    }

// ----------------------------------------------------------------------------
// wxArtProvider::InitStdProvider
// ----------------------------------------------------------------------------

/*static*/ void wxArtProvider::InitStdProvider()
{
    wxArtProvider::Push(new wxDefaultArtProvider);
}

#if !defined(__WXGTK20__) || defined(__WXUNIVERSAL__)
/*static*/ void wxArtProvider::InitNativeProvider()
{
}
#endif


// ----------------------------------------------------------------------------
// XPMs with the art
// ----------------------------------------------------------------------------


#if defined(__WXGTK__)
    #include "wx_gtk/info.xpm"
    #include "wx_gtk/error.xpm"
    #include "wx_gtk/warning.xpm"
    #include "wx_gtk/question.xpm"
#elif defined(__WXMOTIF__)
    #include "wx_motif/info.xpm"
    #include "wx_motif/error.xpm"
    #include "wx_motif/warning.xpm"
    #include "wx_motif/question.xpm"
#endif

#if wxUSE_HTML
    #include "wx_htmsidep.xpm"
    #include "wx_htmoptns.xpm"
    #include "wx_htmbook.xpm"
    #include "wx_htmfoldr.xpm"
    #include "wx_htmpage.xpm"
#endif // wxUSE_HTML

#include "wx_missimg.xpm"
#include "wx_addbookm.xpm"
#include "wx_delbookm.xpm"
#include "wx_back.xpm"
#include "wx_forward.xpm"
#include "wx_up.xpm"
#include "wx_down.xpm"
#include "wx_toparent.xpm"
#include "wx_fileopen.xpm"
#include "wx_print.xpm"
#include "wx_helpicon.xpm"
#include "wx_tipicon.xpm"
#include "wx_home.xpm"
#include "wx_repview.xpm"
#include "wx_listview.xpm"
#include "wx_new_dir.xpm"
#include "wx_harddisk.xpm"
#include "wx_cdrom.xpm"
#include "wx_floppy.xpm"
#include "wx_removable.xpm"
#include "wx_folder.xpm"
#include "wx_folder_open.xpm"
#include "wx_dir_up.xpm"
#include "wx_exefile.xpm"
#include "wx_deffile.xpm"
#include "wx_tick.xpm"
#include "wx_cross.xpm"

#include "wx_filesave.xpm"
#include "wx_filesaveas.xpm"
#include "wx_copy.xpm"
#include "wx_cut.xpm"
#include "wx_paste.xpm"
#include "wx_delete.xpm"
#include "wx_new.xpm"
#include "wx_undo.xpm"
#include "wx_redo.xpm"
#include "wx_quit.xpm"
#include "wx_find.xpm"
#include "wx_findrepl.xpm"



wxBitmap wxDefaultArtProvider_CreateBitmap(const wxArtID& id)
{
    // wxMessageBox icons:
    ART_MSGBOX(wxART_ERROR,       wxICON_ERROR,       error)
    ART_MSGBOX(wxART_INFORMATION, wxICON_INFORMATION, info)
    ART_MSGBOX(wxART_WARNING,     wxICON_WARNING,     warning)
    ART_MSGBOX(wxART_QUESTION,    wxICON_QUESTION,    question)

    // standard icons:
#if wxUSE_HTML
    ART(wxART_HELP_SIDE_PANEL,                     htmsidep)
    ART(wxART_HELP_SETTINGS,                       htmoptns)
    ART(wxART_HELP_BOOK,                           htmbook)
    ART(wxART_HELP_FOLDER,                         htmfoldr)
    ART(wxART_HELP_PAGE,                           htmpage)
#endif // wxUSE_HTML
    ART(wxART_MISSING_IMAGE,                       missimg)
    ART(wxART_ADD_BOOKMARK,                        addbookm)
    ART(wxART_DEL_BOOKMARK,                        delbookm)
    ART(wxART_GO_BACK,                             back)
    ART(wxART_GO_FORWARD,                          forward)
    ART(wxART_GO_UP,                               up)
    ART(wxART_GO_DOWN,                             down)
    ART(wxART_GO_TO_PARENT,                        toparent)
    ART(wxART_GO_HOME,                             home)
    ART(wxART_FILE_OPEN,                           fileopen)
    ART(wxART_PRINT,                               print)
    ART(wxART_HELP,                                helpicon)
    ART(wxART_TIP,                                 tipicon)
    ART(wxART_REPORT_VIEW,                         repview)
    ART(wxART_LIST_VIEW,                           listview)
    ART(wxART_NEW_DIR,                             new_dir)
    ART(wxART_HARDDISK,                            harddisk)
    ART(wxART_FLOPPY,                              floppy)
    ART(wxART_CDROM,                               cdrom)
    ART(wxART_REMOVABLE,                           removable)
    ART(wxART_FOLDER,                              folder)
    ART(wxART_FOLDER_OPEN,                         folder_open)
    ART(wxART_GO_DIR_UP,                           dir_up)
    ART(wxART_EXECUTABLE_FILE,                     exefile)
    ART(wxART_NORMAL_FILE,                         deffile)
    ART(wxART_TICK_MARK,                           tick)
    ART(wxART_CROSS_MARK,                          cross)

    ART(wxART_FILE_SAVE,                           filesave)
    ART(wxART_FILE_SAVE_AS,                        filesaveas)
    ART(wxART_COPY,                                copy)
    ART(wxART_CUT,                                 cut)
    ART(wxART_PASTE,                               paste)
    ART(wxART_DELETE,                              delete)
    ART(wxART_UNDO,                                undo)
    ART(wxART_REDO,                                redo)
    ART(wxART_QUIT,                                quit)
    ART(wxART_FIND,                                find)
    ART(wxART_FIND_AND_REPLACE,                    findrepl)
    ART(wxART_NEW,                                 new)


    return wxNullBitmap;
}

// ----------------------------------------------------------------------------
// CreateBitmap routine
// ----------------------------------------------------------------------------

wxBitmap wxDefaultArtProvider::CreateBitmap(const wxArtID& id,
                                            const wxArtClient& client,
                                            const wxSize& reqSize)
{
    wxBitmap bmp = wxDefaultArtProvider_CreateBitmap(id);

#if wxUSE_IMAGE && (!defined(__WXMSW__) || wxUSE_WXDIB)
    if (bmp.Ok())
    {
        // fit into transparent image with desired size hint from the client
        if (reqSize == wxDefaultSize)
        {
            // find out if there is a desired size for this client
            wxSize bestSize = GetSizeHint(client);
            if (bestSize != wxDefaultSize)
            {
                int bmp_w = bmp.GetWidth();
                int bmp_h = bmp.GetHeight();

                if ((bmp_h < bestSize.x) && (bmp_w < bestSize.y))
                {
                    // the caller wants default size, which is larger than 
                    // the image we have; to avoid degrading it visually by
                    // scaling it up, paste it into transparent image instead:
                    wxPoint offset((bestSize.x - bmp_w)/2, (bestSize.y - bmp_h)/2);
                    wxImage img = bmp.ConvertToImage();
                    img.Resize(bestSize, offset);
                    bmp = wxBitmap(img);
                }
                else // scale (down or mixed, but not up)
                {
                    wxImage img = bmp.ConvertToImage();
                    bmp = wxBitmap
                          (
                              img.Scale(bestSize.x, bestSize.y,
                                        wxIMAGE_QUALITY_HIGH)
                          );
                }
            }
        }
    }
#else
    wxUnusedVar(client);
    wxUnusedVar(reqSize);
#endif // wxUSE_IMAGE

    return bmp;
}
