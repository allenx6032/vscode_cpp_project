
/* spew out a thoroughly gigantic file designed so that bzip2
   can compress it reasonably rapidly.  This is to help test
   support for large files (> 2GB) in a reasonable amount of time.
   I suggest you use the undocumented --exponential option to
   bzip2 when compressing the resulting file; this saves a bit of
   time.  Note: *don't* bother with --exponential when compressing
   Real Files; it'll just waste a lot of CPU time :-)
   (but is otherwise harmless).
*/

/* ------------------------------------------------------------------
   This file is part of bzip2/libbzip2, a program and library for
   lossless, block-sorting data compression.

   bzip2/libbzip2 version 1.1.0 of 6 September 2010
   Copyright (C) 1996-2010 Julian Seward <jseward@acm.org>

   Please read the WARNING, DISCLAIMER and PATENTS sections in the
   README file.

   This program is released under the terms of the license contained
   in the file LICENSE.
------------------------------------------------------------------ */


#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>

/* The number of megabytes of junk to spew out (roughly) */
#define MEGABYTES 5000

#define N_BUF 1000000
char buf[N_BUF];

// #ifdef _WIN32
// #else
// #endif
int main ( int argc, char** argv )
{
   int ii, kk, p;
#ifdef _WIN32
   srand(1);
#else
   srandom(1);
#endif
   setvbuf(stdout, buf, _IOFBF, sizeof(buf)); 
#ifdef _WIN32
#else
   setbuffer ( stdout, buf, N_BUF );
#endif
   for (kk = 0; kk < MEGABYTES * 515; kk+=3) {
      #ifdef _WIN32
      p = 25+rand()%50;
      #else
      p = 25+random()%50;
      #endif
      for (ii = 0; ii < p; ii++)
         printf ( "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" );
      for (ii = 0; ii < p-1; ii++)
         printf ( "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" );
      for (ii = 0; ii < p+1; ii++)
         printf ( "ccccccccccccccccccccccccccccccccccccc" );
   }
   fflush(stdout);
   return 0;
}
