/* rlmbutil.h -- utility functions for multibyte characters. */

/* Copyright (C) 2001, 2003 Free Software Foundation, Inc.

   This file is part of the GNU Readline Library, a library for
   reading lines of text with interactive input and history editing.

   The GNU Readline Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2, or
   (at your option) any later version.

   The GNU Readline Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   The GNU General Public License is often shipped with GNU software, and
   is generally kept in a file called COPYING or LICENSE.  If you do not
   have a copy of the license, write to the Free Software Foundation,
   59 Temple Place, Suite 330, Boston, MA 02111 USA. */

#if !defined (_RL_MBUTIL_H_)
#define _RL_MBUTIL_H_

#include "readline_rlstdc.h"

/************************************************/
/* check multibyte capability for I18N code     */
/************************************************/

/* For platforms which support the ISO C amendement 1 functionality we
   support user defined character classes.  */
   /* Solaris 2.5 has a bug: <wchar.h> must be included before <wctype.h>.  */
// #if defined (HAVE_WCTYPE_H) && defined (HAVE_WCHAR_H)
// #  include <wchar.h>
// #  include <wctype.h>
// #  if defined (HAVE_MBSRTOWCS) && defined (HAVE_MBRTOWC) && defined (HAVE_MBRLEN) && defined (HAVE_WCWIDTH)
//      /* system is supposed to support XPG5 */
// #    define HANDLE_MULTIBYTE      1
// #  endif
// #endif

/* If we don't want multibyte chars even on a system that supports them, let
   the configuring user turn multibyte support off. */
#if defined (NO_MULTIBYTE_SUPPORT)
#  undef HANDLE_MULTIBYTE
#endif

/* Some systems, like BeOS, have multibyte encodings but lack mbstate_t.  */
#if HANDLE_MULTIBYTE && !defined (HAVE_MBSTATE_T)
#  define wcsrtombs(dest, src, len, ps) (wcsrtombs) (dest, src, len, 0)
#  define mbsrtowcs(dest, src, len, ps) (mbsrtowcs) (dest, src, len, 0)
#  define wcrtomb(s, wc, ps) (wcrtomb) (s, wc, 0)
#  define mbrtowc(pwc, s, n, ps) (mbrtowc) (pwc, s, n, 0)
#  define mbrlen(s, n, ps) (mbrlen) (s, n, 0)
#  define mbstate_t int
#endif

/* Make sure MB_LEN_MAX is at least 16 on systems that claim to be able to
   handle multibyte chars (some systems define MB_LEN_MAX as 1) */
#ifdef HANDLE_MULTIBYTE
#  include <limits.h>
#  if defined(MB_LEN_MAX) && (MB_LEN_MAX < 16)
#    undef MB_LEN_MAX
#  endif
#  if !defined (MB_LEN_MAX)
#    define MB_LEN_MAX 16
#  endif
#endif

#ifdef HANDLE_MULTIBYTE
int wcwidth(wchar_t ucs);
int wcswidth(const wchar_t *pwcs, size_t n);
int wcswidth_cjk(const wchar_t *pwcs, size_t n);
#endif

/************************************************/
/* end of multibyte capability checks for I18N  */
/************************************************/

/*
 * Flags for _rl_find_prev_mbchar and _rl_find_next_mbchar:
 *
 * MB_FIND_ANY		find any multibyte character
 * MB_FIND_NONZERO	find a non-zero-width multibyte character
 */

#define MB_FIND_ANY	0x00
#define MB_FIND_NONZERO	0x01

READLINE_DLL_IMPEXP int _rl_find_prev_mbchar PARAMS((char *, int, int));
READLINE_DLL_IMPEXP int _rl_find_next_mbchar PARAMS((char *, int, int, int));

#ifdef HANDLE_MULTIBYTE

READLINE_DLL_IMPEXP int _rl_compare_chars PARAMS((char *, int, mbstate_t *, char *, int, mbstate_t *));
READLINE_DLL_IMPEXP int _rl_get_char_len PARAMS((char *, mbstate_t *));
READLINE_DLL_IMPEXP int _rl_adjust_point PARAMS((char *, int, mbstate_t *));

READLINE_DLL_IMPEXP int _rl_read_mbchar PARAMS((char *, int));
READLINE_DLL_IMPEXP int _rl_read_mbstring PARAMS((int, char *, int));

READLINE_DLL_IMPEXP int _rl_is_mbchar_matched PARAMS((char *, int, int, char *, int));

#define MB_INVALIDCH(x)		((x) == (size_t)-1 || (x) == (size_t)-2)
#define MB_NULLWCH(x)		((x) == 0)

#else /* !HANDLE_MULTIBYTE */

#undef MB_LEN_MAX
#undef MB_CUR_MAX

#define MB_LEN_MAX	1
#define MB_CUR_MAX	1

#define _rl_find_prev_mbchar(b, i, f)		(((i) == 0) ? (i) : ((i) - 1))
#define _rl_find_next_mbchar(b, i1, i2, f)	((i1) + (i2))

#define MB_INVALIDCH(x)		(0)
#define MB_NULLWCH(x)		(0)

#endif /* !HANDLE_MULTIBYTE */

READLINE_EXTERN int rl_byte_oriented;

#endif /* _RL_MBUTIL_H_ */
