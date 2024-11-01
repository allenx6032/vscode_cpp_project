//
// "$Id$"
//
// File chooser widget for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2015 by Bill Spitzak and others.
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

#include "fl_flstring.h"
#include <fl_filename.H>
#include <Fl_File_Chooser.H>
#include <fl_ask.H>


static Fl_File_Chooser	*fc = (Fl_File_Chooser *)0;
static void		(*current_callback)(const char*) = 0;
static const char	*current_label = fl_ok;


// Do a file chooser callback...
static void callback(Fl_File_Chooser *, void*) {
  if (current_callback && fc->value())
    (*current_callback)(fc->value());
}

// Pop up a file chooser dialog window and wait until it is closed...
static void popup(Fl_File_Chooser *fc) {
  fc->show();

  // deactivate Fl::grab(), because it is incompatible with modal windows
  Fl_Window* g = Fl::grab();
  if (g) Fl::grab(0);

  while (fc->shown())
    Fl::wait();

  if (g) // regrab the previous popup menu, if there was one
    Fl::grab(g);
}


/** \addtogroup  group_comdlg 
    @{ */

/** 
    Set the file chooser callback
    \note \#include <Fl_File_Chooser.H>
    \relates Fl_File_Chooser
*/
void fl_file_chooser_callback(void (*cb)(const char*)) {
  current_callback = cb;
}


/** 
    Set the "OK" button label
    \note \#include <Fl_File_Chooser.H>
    \relates Fl_File_Chooser
*/
void fl_file_chooser_ok_label(const char *l) {
  if (l) current_label = l;
  else current_label = fl_ok;
}

/** 
    Shows a file chooser dialog and gets a filename. 
    \note \#include <Fl_File_Chooser.H>
    \image html Fl_File_Chooser.jpg 
    \image latex  Fl_File_Chooser.jpg "Fl_File_Chooser" width=12cm
    \param[in] message text in title bar
    \param[in] pat filename pattern filter
    \param[in] fname initial/default filename selection
    \param[in] relative 0 for absolute path name, relative path name otherwise
    \return the user selected filename, in absolute or relative format 
            or NULL if user cancels
    \relates Fl_File_Chooser
*/
char *					// O - Filename or NULL
fl_file_chooser(const char *message,	// I - Message in titlebar
                const char *pat,	// I - Filename pattern
		const char *fname,	// I - Initial filename selection
		int        relative) {	// I - 0 for absolute path
  static char	retname[FL_PATH_MAX];		// Returned filename

  if (!fc) {
    if (!fname || !*fname) fname = ".";

    fc = new Fl_File_Chooser(fname, pat, Fl_File_Chooser::CREATE, message);
    fc->callback(callback, 0);
  } else {
    fc->type(Fl_File_Chooser::CREATE);
    // see, if we use the same pattern between calls
    char same_pattern = 0;
    const char *fcf = fc->filter();
    if ( fcf && pat && strcmp(fcf, pat)==0)
      same_pattern = 1;
    else if ( (fcf==0L || *fcf==0) && (pat==0L || *pat==0) )
      same_pattern = 1;
    // now set the pattern to the new pattern (even if they are the same)
    fc->filter(pat);
    fc->label(message);

    if (!fname) { // null pointer reuses same filename if pattern didn't change
      if (!same_pattern && fc->value()) {
	// if pattern is different, remove name but leave old directory:
	strlcpy(retname, fc->value(), sizeof(retname));

	char *p = strrchr(retname, '/');

        if (p) {
	  // If the filename is "/foo", then the directory will be "/", not
	  // ""...
	  if (p == retname)
	    retname[1] = '\0';
	  else
	    *p = '\0';
	}
	// Set the directory...
	fc->value(retname);
      } else {
        // re-use the previously selected name
      }
    } else if (!*fname) { // empty filename reuses directory with empty name
      const char *fcv = fc->value();
      if (fcv) 
        strlcpy(retname, fc->value(), sizeof(retname));
      else 
        *retname = 0;
      const char *n = fl_filename_name(retname);
      if (n) *((char*)n) = 0;
      fc->value("");
      fc->directory(retname);
    } else {
       fc->value(fname);
    }
  }

  fc->ok_label(current_label);
  popup(fc);
  if (fc->value() && relative) {
    fl_filename_relative(retname, sizeof(retname), fc->value());

    return retname;
  } else if (fc->value()) return (char *)fc->value();
  else return 0;
}

/** Shows a file chooser dialog and gets a directory.
    \note \#include <Fl_File_Chooser.H>
    \param[in] message title bar text
    \param[in] fname initial/default directory name
    \param[in] relative 0 for absolute path return, relative otherwise
    \return the directory path string chosen by the user or NULL if user cancels
    \relates Fl_File_Chooser
*/
char *					// O - Directory or NULL
fl_dir_chooser(const char *message,	// I - Message for titlebar
               const char *fname,	// I - Initial directory name
	       int        relative)	// I - 0 for absolute
{
  static char	retname[FL_PATH_MAX];		// Returned directory name

  if (!fc) {
    if (!fname || !*fname) fname = ".";

    fc = new Fl_File_Chooser(fname, "*", Fl_File_Chooser::CREATE |
                                         Fl_File_Chooser::DIRECTORY, message);
    fc->callback(callback, 0);
  } else {
    fc->type(Fl_File_Chooser::CREATE | Fl_File_Chooser::DIRECTORY);
    fc->filter("*");
    if (fname && *fname) fc->value(fname);
    fc->label(message);
  }

  popup(fc);

  if (fc->value() && relative) {
    fl_filename_relative(retname, sizeof(retname), fc->value());

    return retname;
  } else if (fc->value()) return (char *)fc->value();
  else return 0;
}
/** @} */


//
// End of "$Id$".
//
