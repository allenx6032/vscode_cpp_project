//
// "$Id$"
//
// Mouse cursor support for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2012 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

// Change the current cursor.
// Under X the cursor is attached to the X window.  I tried to hide
// this and pretend that changing the cursor is a drawing function.
// This avoids a field in the Fl_Window, and I suspect is more
// portable to other systems.

#include <Fl.H>
#include <Fl_Window.H>
#include <Fl_Pixmap.H>
#include <Fl_RGB_Image.H>
#include <fl_x.H>
#include <fl_draw.H>

#include "fl_cursor_wait.xpm"
#include "fl_cursor_help.xpm"
#include "fl_cursor_nwse.xpm"
#include "fl_cursor_nesw.xpm"
#include "fl_cursor_none.xpm"

/**
  Sets the cursor for the current window to the specified shape and colors.
  The cursors are defined in the <FL/Enumerations.H> header file. 
  */
void fl_cursor(Fl_Cursor c) {
  if (Fl::first_window()) Fl::first_window()->cursor(c);
}

/* For back compatibility only. */
void fl_cursor(Fl_Cursor c, Fl_Color fg, Fl_Color bg) {
  fl_cursor(c);
}


/** 
    Sets the default window cursor. This is the cursor that will be used
    after the mouse pointer leaves a widget with a custom cursor set.

    \see cursor(const Fl_RGB_Image*, int, int), default_cursor()
*/
void Fl_Window::default_cursor(Fl_Cursor c) {
  cursor_default = c;
  cursor(c);
}


static void fallback_cursor(Fl_Window *w, Fl_Cursor c) {
  const char **xpm;
  int hotx, hoty;

  // The standard arrow is our final fallback, so something is broken
  // if we get called back here with that as an argument.
  if (c == FL_CURSOR_ARROW)
    return;

  switch (c) {
  case FL_CURSOR_WAIT:
    xpm = (const char**)fl_cursor_wait_xpm;
    hotx = 7;
    hoty = 9;
    break;
  case FL_CURSOR_HELP:
    xpm = (const char**)fl_cursor_help_xpm;
    hotx = 1;
    hoty = 3;
    break;
  case FL_CURSOR_NWSE:
    xpm = (const char**)fl_cursor_nwse_xpm;
    hotx = 7;
    hoty = 7;
    break;
  case FL_CURSOR_NESW:
    xpm = (const char**)fl_cursor_nesw_xpm;
    hotx = 7;
    hoty = 7;
    break;
  case FL_CURSOR_NONE:
    xpm = (const char**)fl_cursor_none_xpm;
    hotx = 0;
    hoty = 0;
    break;
  default:
    w->cursor(FL_CURSOR_ARROW);
    return;
  }

  Fl_Pixmap pxm(xpm);
  Fl_RGB_Image image(&pxm);

  w->cursor(&image, hotx, hoty);
}


void Fl_Window::cursor(Fl_Cursor c) {
  int ret;

  // the cursor must be set for the top level window, not for subwindows
  Fl_Window *w = window(), *toplevel = this;

  while (w) {
    toplevel = w;
    w = w->window();
  }

  if (toplevel != this) {
    toplevel->cursor(c);
    return;
  }

  if (c == FL_CURSOR_DEFAULT)
    c = cursor_default;

  if (!i)
    return;

  ret = i->set_cursor(c);
  if (ret)
    return;

  fallback_cursor(this, c);
}

/**
  Changes the cursor for this window.  This always calls the system, if
  you are changing the cursor a lot you may want to keep track of how
  you set it in a static variable and call this only if the new cursor
  is different.

  The default cursor will be used if the provided image cannot be used
  as a cursor.

  \see cursor(Fl_Cursor), default_cursor()
*/
void Fl_Window::cursor(const Fl_RGB_Image *image, int hotx, int hoty) {
  int ret;

  // the cursor must be set for the top level window, not for subwindows
  Fl_Window *w = window(), *toplevel = this;

  while (w) {
    toplevel = w;
    w = w->window();
  }

  if (toplevel != this) {
    toplevel->cursor(image, hotx, hoty);
    return;
  }

  if (!i)
    return;

  ret = i->set_cursor(image, hotx, hoty);
  if (ret)
    return;

  cursor(FL_CURSOR_DEFAULT);
}

/**
 For back compatibility only.
 Same as Fl_Window::cursor(Fl_Cursor) 
*/
void Fl_Window::cursor(Fl_Cursor c, Fl_Color, Fl_Color) {
  cursor(c);
};

/** 
 For back compatibility only.
 same as Fl_Window::default_cursor(Fl_Cursor) 
*/
void Fl_Window::default_cursor(Fl_Cursor c, Fl_Color, Fl_Color) {
  default_cursor(c);
};


//
// End of "$Id$".
//
