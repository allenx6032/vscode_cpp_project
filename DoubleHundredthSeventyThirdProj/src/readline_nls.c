/* nls.c -- skeletal internationalization code. */

/* Copyright (C) 1996 Free Software Foundation, Inc.

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
#define READLINE_LIBRARY

#if defined (HAVE_READLINE_CONFIG_H)
#  include <readline_config.h>
#endif

#include <sys/types.h>

#include <stdio.h>

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#endif /* HAVE_UNISTD_H */

#if defined (HAVE_STDLIB_H)
#  include <stdlib.h>
#else
#  include "readline_ansi_stdlib.h"
#endif /* HAVE_STDLIB_H */

#if defined (HAVE_LOCALE_H)
#  include <locale.h>
#endif

#include <ctype.h>

#include "readline_rldefs.h"
#include "readline.h"
#include "readline_rlshell.h"
#include "readline_rlprivate.h"

#if !defined (HAVE_SETLOCALE)    
/* A list of legal values for the LANG or LC_CTYPE environment variables.
   If a locale name in this list is the value for the LC_ALL, LC_CTYPE,
   or LANG environment variable (using the first of those with a value),
   readline eight-bit mode is enabled. */
static char *legal_lang_values[] =
{
 "iso88591",
 "iso88592",
 "iso88593",
 "iso88594",
 "iso88595",
 "iso88596",
 "iso88597",
 "iso88598",
 "iso88599",
 "iso885910",
 "koi8r",
  0
};

static char *normalize_codeset PARAMS((char *));
static char *find_codeset PARAMS((char *, size_t *));
#endif /* !HAVE_SETLOCALE */

static char *_rl_get_locale_var PARAMS((const char *));

static char *
_rl_get_locale_var (v)
     const char *v;
{
  char *lspec;

  lspec = sh_get_env_value ("LC_ALL");
  if (lspec == 0 || *lspec == 0)
    lspec = sh_get_env_value (v);
  if (lspec == 0 || *lspec == 0)
    lspec = sh_get_env_value ("LANG");

  return lspec;
}
  
/* Check for LC_ALL, LC_CTYPE, and LANG and use the first with a value
   to decide the defaults for 8-bit character input and output.  Returns
   1 if we set eight-bit mode. */
int
_rl_init_eightbit ()
{
/* If we have setlocale(3), just check the current LC_CTYPE category
   value, and go into eight-bit mode if it's not C or POSIX. */
#if defined (HAVE_SETLOCALE)
  char *lspec, *t;

  /* Set the LC_CTYPE locale category from environment variables. */
  lspec = _rl_get_locale_var ("LC_CTYPE");
  /* Since _rl_get_locale_var queries the right environment variables,
     we query the current locale settings with setlocale(), and, if
     that doesn't return anything, we set lspec to the empty string to
     force the subsequent call to setlocale() to define the `native'
     environment. */
  if (lspec == 0 || *lspec == 0)
    lspec = setlocale (LC_CTYPE, (char *)NULL);
  if (lspec == 0)
    lspec = "";
  t = setlocale (LC_CTYPE, lspec);

#ifndef _WIN32
  if (t && *t && (t[0] != 'C' || t[1]) && (STREQ (t, "POSIX") == 0))
    {
      _rl_meta_flag = 1;
      _rl_convert_meta_chars_to_ascii = 0;
      _rl_output_meta_chars = 1;
      return (1);
    }
  else
#endif
    return (0);

#else /* !HAVE_SETLOCALE */
  char *lspec, *t;
  int i;

  /* We don't have setlocale.  Finesse it.  Check the environment for the
     appropriate variables and set eight-bit mode if they have the right
     values. */
  lspec = _rl_get_locale_var ("LC_CTYPE");

  if (lspec == 0 || (t = normalize_codeset (lspec)) == 0)
    return (0);
  for (i = 0; t && legal_lang_values[i]; i++)
    if (STREQ (t, legal_lang_values[i]))
      {
	_rl_meta_flag = 1;
	_rl_convert_meta_chars_to_ascii = 0;
	_rl_output_meta_chars = 1;
	break;
      }
  free (t);
  return (legal_lang_values[i] ? 1 : 0);

#endif /* !HAVE_SETLOCALE */
}

#if !defined (HAVE_SETLOCALE)
static char *
normalize_codeset (codeset)
     char *codeset;
{
  size_t namelen, i;
  int len, all_digits;
  char *wp, *retval;

  codeset = find_codeset (codeset, &namelen);

  if (codeset == 0)
    return (codeset);

  all_digits = 1;
  for (len = 0, i = 0; i < namelen; i++)
    {
      if (ISALNUM ((unsigned char)codeset[i]))
	{
	  len++;
	  all_digits &= _rl_digit_p (codeset[i]);
	}
    }

  retval = (char *)malloc ((all_digits ? 3 : 0) + len + 1);
  if (retval == 0)
    return ((char *)0);

  wp = retval;
  /* Add `iso' to beginning of an all-digit codeset */
  if (all_digits)
    {
      *wp++ = 'i';
      *wp++ = 's';
      *wp++ = 'o';
    }

  for (i = 0; i < namelen; i++)
    if (ISALPHA ((unsigned char)codeset[i]))
      *wp++ = _rl_to_lower (codeset[i]);
    else if (_rl_digit_p (codeset[i]))
      *wp++ = codeset[i];
  *wp = '\0';

  return retval;
}

/* Isolate codeset portion of locale specification. */
static char *
find_codeset (name, lenp)
     char *name;
     size_t *lenp;
{
  char *cp, *language, *result;

  cp = language = name;
  result = (char *)0;

  while (*cp && *cp != '_' && *cp != '@' && *cp != '+' && *cp != ',')
    cp++;

  /* This does not make sense: language has to be specified.  As
     an exception we allow the variable to contain only the codeset
     name.  Perhaps there are funny codeset names.  */
  if (language == cp) 
    {
      *lenp = strlen (language);
      result = language;
    }
  else
    {
      /* Next is the territory. */
      if (*cp == '_')
	do
	  ++cp;
	while (*cp && *cp != '.' && *cp != '@' && *cp != '+' && *cp != ',' && *cp != '_');

      /* Now, finally, is the codeset. */
      result = cp;
      if (*cp == '.')
	do
	  ++cp;
	while (*cp && *cp != '@');

      if (cp - result > 2)
	{
	  result++;
	  *lenp = cp - result;
	}
      else
	{
	  *lenp = strlen (language);
	  result = language;
	}
    }

  return result;
}
#endif /* !HAVE_SETLOCALE */
