/*
 * "$Id$"
 *
 * Copyright 1998-2010 by Bill Spitzak and others.
 *
 * fl_call_main() calls main() for you Windows people.  Needs to be done in C
 * because Borland C++ won't let you call main() from C++.
 *
 * This library is free software. Distribution and use rights are outlined in
 * the file "COPYING" which should have been included with this file.  If this
 * file is missing or damaged, see the license at:
 *
 *     http://www.fltk.org/COPYING.php
 *
 * Please report all bugs and problems on the following page:
 *
 *     http://www.fltk.org/str.php
 */

/*
 * This WinMain() function can be overridden by an application and
 * is provided for compatibility with programs written for other
 * operating systems that conform to the ANSI standard entry point
 * "main()".  This will allow you to build a WIN32 Application
 * without any special settings.
 *
 * Because of problems with the Microsoft Visual C++ header files
 * and/or compiler, you cannot have a WinMain function in a DLL.
 * I don't know why.  Thus, this nifty feature is only available
 * if you link to the static library.
 *
 * Currently the debug version of this library will create a
 * console window for your application so you can put printf()
 * statements for debugging or informational purposes.  Ultimately
 * we want to update this to always use the parent's console,
 * but at present we have not identified a function or API in
 * Microsoft(r) Windows(r) that allows for it.
 */

#if defined(WIN32) && !defined(FL_DLL) && !defined (__GNUC__)

#  include <windows.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <fl_utf8.h>

extern int main(int, char *[]);

#  ifdef BORLAND5
#    define __argc _argc
#    define __argv _argv
#  endif /* BORLAND5 */

/* static int mbcs2utf(const char *s, int l, char *dst, unsigned dstlen) */
static int mbcs2utf(const char *s, int l, char *dst)
{
  static xchar *mbwbuf;
  unsigned dstlen = 0;
  if (!s) return 0;
  dstlen = (l * 6) + 6;
  mbwbuf = (xchar*)malloc(dstlen * sizeof(xchar));
  l = (int) mbstowcs(mbwbuf, s, l);
/* l = fl_unicode2utf(mbwbuf, l, dst); */
  l = fl_utf8fromwc(dst, dstlen, mbwbuf, l);
  dst[l] = 0;
  free(mbwbuf);
  return l;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                             LPSTR lpCmdLine, int nCmdShow) {
  int rc, i;
  char **ar;

#  ifdef _DEBUG
 /*
  * If we are using compiling in debug mode, open a console window so
  * we can see any printf's, etc...
  *
  * While we can detect if the program was run from the command-line -
  * look at the CMDLINE environment variable, it will be "WIN" for
  * programs started from the GUI - the shell seems to run all WIN32
  * applications in the background anyways...
  */

  AllocConsole();
  freopen("conin$", "r", stdin);
  freopen("conout$", "w", stdout);
  freopen("conout$", "w", stderr);
#  endif /* _DEBUG */

  ar = (char**) malloc(sizeof(char*) * (__argc + 1));
  i = 0;
  while (i < __argc) {
    int l;
    unsigned dstlen;
    if (__wargv ) {
      for (l = 0; __wargv[i] && __wargv[i][l]; l++) {}; /* is this just wstrlen??? */
      dstlen = (l * 5) + 1;
      ar[i] = (char*) malloc(dstlen);
/*    ar[i][fl_unicode2utf(__wargv[i], l, ar[i])] = 0; */
      dstlen = fl_utf8fromwc(ar[i], dstlen, __wargv[i], l);
      ar[i][dstlen] = 0;
    } else {
      for (l = 0; __argv[i] && __argv[i][l]; l++) {};
      dstlen = (l * 5) + 1;
      ar[i] = (char*) malloc(dstlen);
/*      ar[i][mbcs2utf(__argv[i], l, ar[i], dstlen)] = 0; */
      ar[i][mbcs2utf(__argv[i], l, ar[i])] = 0;
    }
    i++;
  }
  ar[__argc] = 0;
  /* Run the standard main entry point function... */
  rc = main(__argc, ar);

#  ifdef _DEBUG
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
#  endif /* _DEBUG */

  return rc;
}

#else
/* STR# 2973: solves "empty translation unit" error (Sun, HP-UX..) */
typedef int dummy;
#endif /* WIN32 && !FL_DLL && !__GNUC__ */

/*
 * End of "$Id$".
 */

