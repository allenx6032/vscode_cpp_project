/*************************************************
*      Perl-Compatible Regular Expressions       *
*************************************************/

/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2012 University of Cambridge

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/


/* This module contains the external function pcre_compile(), along with
supporting internal functions that are not used by other modules. */


#ifdef HAVE_PCRE_CONFIG_H
#include "pcre_config.h"
#endif

#define NLBLOCK cd             /* Block containing newline information */
#define PSSTART start_pattern  /* Field containing processed string start */
#define PSEND   end_pattern    /* Field containing processed string end */

#include "pcre_internal.h"


/* When PCRE_DEBUG is defined, we need the pcre(16)_printint() function, which
is also used by pcretest. PCRE_DEBUG is not defined when building a production
library. We do not need to select pcre16_printint.c specially, because the
COMPILE_PCREx macro will already be appropriately set. */

#ifdef PCRE_DEBUG
/* pcre_printint.c should not include any headers */
#define PCRE_INCLUDED
#include "pcre_printint.c"
#undef PCRE_INCLUDED
#endif


/* Macro for setting individual bits in class bitmaps. */

#define SETBIT(a,b) a[b/8] |= (1 << (b%8))

/* Maximum length value to check against when making sure that the integer that
holds the compiled pattern length does not overflow. We make it a bit less than
INT_MAX to allow for adding in group terminating bytes, so that we don't have
to check them every time. */

#define OFLOW_MAX (INT_MAX - 20)


/*************************************************
*      Code parameters and static tables         *
*************************************************/

/* This value specifies the size of stack workspace that is used during the
first pre-compile phase that determines how much memory is required. The regex
is partly compiled into this space, but the compiled parts are discarded as
soon as they can be, so that hopefully there will never be an overrun. The code
does, however, check for an overrun. The largest amount I've seen used is 218,
so this number is very generous.

The same workspace is used during the second, actual compile phase for
remembering forward references to groups so that they can be filled in at the
end. Each entry in this list occupies LINK_SIZE bytes, so even when LINK_SIZE
is 4 there is plenty of room for most patterns. However, the memory can get
filled up by repetitions of forward references, for example patterns like
/(?1){0,1999}(b)/, and one user did hit the limit. The code has been changed so
that the workspace is expanded using malloc() in this situation. The value
below is therefore a minimum, and we put a maximum on it for safety. The
minimum is now also defined in terms of LINK_SIZE so that the use of malloc()
kicks in at the same number of forward references in all cases. */

#define COMPILE_WORK_SIZE (2048*LINK_SIZE)
#define COMPILE_WORK_SIZE_MAX (100*COMPILE_WORK_SIZE)

/* The overrun tests check for a slightly smaller size so that they detect the
overrun before it actually does run off the end of the data block. */

#define WORK_SIZE_SAFETY_MARGIN (100)

/* Private flags added to firstchar and reqchar. */

#define REQ_CASELESS   0x10000000l      /* Indicates caselessness */
#define REQ_VARY       0x20000000l      /* Reqchar followed non-literal item */

/* Repeated character flags. */

#define UTF_LENGTH     0x10000000l      /* The char contains its length. */

/* Table for handling escaped characters in the range '0'-'z'. Positive returns
are simple data values; negative values are for special things like \d and so
on. Zero means further processing is needed (for things like \x), or the escape
is invalid. */

#ifndef EBCDIC

/* This is the "normal" table for ASCII systems or for EBCDIC systems running
in UTF-8 mode. */

static const short int escapes[] = {
     0,                       0,
     0,                       0,
     0,                       0,
     0,                       0,
     0,                       0,
     CHAR_COLON,              CHAR_SEMICOLON,
     CHAR_LESS_THAN_SIGN,     CHAR_EQUALS_SIGN,
     CHAR_GREATER_THAN_SIGN,  CHAR_QUESTION_MARK,
     CHAR_COMMERCIAL_AT,      -ESC_A,
     -ESC_B,                  -ESC_C,
     -ESC_D,                  -ESC_E,
     0,                       -ESC_G,
     -ESC_H,                  0,
     0,                       -ESC_K,
     0,                       0,
     -ESC_N,                  0,
     -ESC_P,                  -ESC_Q,
     -ESC_R,                  -ESC_S,
     0,                       0,
     -ESC_V,                  -ESC_W,
     -ESC_X,                  0,
     -ESC_Z,                  CHAR_LEFT_SQUARE_BRACKET,
     CHAR_BACKSLASH,          CHAR_RIGHT_SQUARE_BRACKET,
     CHAR_CIRCUMFLEX_ACCENT,  CHAR_UNDERSCORE,
     CHAR_GRAVE_ACCENT,       7,
     -ESC_b,                  0,
     -ESC_d,                  ESC_e,
     ESC_f,                   0,
     -ESC_h,                  0,
     0,                       -ESC_k,
     0,                       0,
     ESC_n,                   0,
     -ESC_p,                  0,
     ESC_r,                   -ESC_s,
     ESC_tee,                 0,
     -ESC_v,                  -ESC_w,
     0,                       0,
     -ESC_z
};

#else

/* This is the "abnormal" table for EBCDIC systems without UTF-8 support. */

static const short int escapes[] = {
/*  48 */     0,     0,      0,     '.',    '<',   '(',    '+',    '|',
/*  50 */   '&',     0,      0,       0,      0,     0,      0,      0,
/*  58 */     0,     0,    '!',     '$',    '*',   ')',    ';',    '~',
/*  60 */   '-',   '/',      0,       0,      0,     0,      0,      0,
/*  68 */     0,     0,    '|',     ',',    '%',   '_',    '>',    '?',
/*  70 */     0,     0,      0,       0,      0,     0,      0,      0,
/*  78 */     0,   '`',    ':',     '#',    '@',  '\'',    '=',    '"',
/*  80 */     0,     7, -ESC_b,       0, -ESC_d, ESC_e,  ESC_f,      0,
/*  88 */-ESC_h,     0,      0,     '{',      0,     0,      0,      0,
/*  90 */     0,     0, -ESC_k,     'l',      0, ESC_n,      0, -ESC_p,
/*  98 */     0, ESC_r,      0,     '}',      0,     0,      0,      0,
/*  A0 */     0,   '~', -ESC_s, ESC_tee,      0,-ESC_v, -ESC_w,      0,
/*  A8 */     0,-ESC_z,      0,       0,      0,   '[',      0,      0,
/*  B0 */     0,     0,      0,       0,      0,     0,      0,      0,
/*  B8 */     0,     0,      0,       0,      0,   ']',    '=',    '-',
/*  C0 */   '{',-ESC_A, -ESC_B,  -ESC_C, -ESC_D,-ESC_E,      0, -ESC_G,
/*  C8 */-ESC_H,     0,      0,       0,      0,     0,      0,      0,
/*  D0 */   '}',     0, -ESC_K,       0,      0,-ESC_N,      0, -ESC_P,
/*  D8 */-ESC_Q,-ESC_R,      0,       0,      0,     0,      0,      0,
/*  E0 */  '\\',     0, -ESC_S,       0,      0,-ESC_V, -ESC_W, -ESC_X,
/*  E8 */     0,-ESC_Z,      0,       0,      0,     0,      0,      0,
/*  F0 */     0,     0,      0,       0,      0,     0,      0,      0,
/*  F8 */     0,     0,      0,       0,      0,     0,      0,      0
};
#endif


/* Table of special "verbs" like (*PRUNE). This is a short table, so it is
searched linearly. Put all the names into a single string, in order to reduce
the number of relocations when a shared library is dynamically linked. The
string is built from string macros so that it works in UTF-8 mode on EBCDIC
platforms. */

typedef struct verbitem {
  int   len;                 /* Length of verb name */
  int   op;                  /* Op when no arg, or -1 if arg mandatory */
  int   op_arg;              /* Op when arg present, or -1 if not allowed */
} verbitem;

static const char verbnames[] =
  "\0"                       /* Empty name is a shorthand for MARK */
  STRING_MARK0
  STRING_ACCEPT0
  STRING_COMMIT0
  STRING_F0
  STRING_FAIL0
  STRING_PRUNE0
  STRING_SKIP0
  STRING_THEN;

static const verbitem verbs[] = {
  { 0, -1,        OP_MARK },
  { 4, -1,        OP_MARK },
  { 6, OP_ACCEPT, -1 },
  { 6, OP_COMMIT, -1 },
  { 1, OP_FAIL,   -1 },
  { 4, OP_FAIL,   -1 },
  { 5, OP_PRUNE,  OP_PRUNE_ARG },
  { 4, OP_SKIP,   OP_SKIP_ARG  },
  { 4, OP_THEN,   OP_THEN_ARG  }
};

static const int verbcount = sizeof(verbs)/sizeof(verbitem);


/* Tables of names of POSIX character classes and their lengths. The names are
now all in a single string, to reduce the number of relocations when a shared
library is dynamically loaded. The list of lengths is terminated by a zero
length entry. The first three must be alpha, lower, upper, as this is assumed
for handling case independence. */

static const char posix_names[] =
  STRING_alpha0 STRING_lower0 STRING_upper0 STRING_alnum0
  STRING_ascii0 STRING_blank0 STRING_cntrl0 STRING_digit0
  STRING_graph0 STRING_print0 STRING_punct0 STRING_space0
  STRING_word0  STRING_xdigit;

static const pcre_uint8 posix_name_lengths[] = {
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 6, 0 };

/* Table of class bit maps for each POSIX class. Each class is formed from a
base map, with an optional addition or removal of another map. Then, for some
classes, there is some additional tweaking: for [:blank:] the vertical space
characters are removed, and for [:alpha:] and [:alnum:] the underscore
character is removed. The triples in the table consist of the base map offset,
second map offset or -1 if no second map, and a non-negative value for map
addition or a negative value for map subtraction (if there are two maps). The
absolute value of the third field has these meanings: 0 => no tweaking, 1 =>
remove vertical space characters, 2 => remove underscore. */

static const int posix_class_maps[] = {
  cbit_word,  cbit_digit, -2,             /* alpha */
  cbit_lower, -1,          0,             /* lower */
  cbit_upper, -1,          0,             /* upper */
  cbit_word,  -1,          2,             /* alnum - word without underscore */
  cbit_print, cbit_cntrl,  0,             /* ascii */
  cbit_space, -1,          1,             /* blank - a GNU extension */
  cbit_cntrl, -1,          0,             /* cntrl */
  cbit_digit, -1,          0,             /* digit */
  cbit_graph, -1,          0,             /* graph */
  cbit_print, -1,          0,             /* print */
  cbit_punct, -1,          0,             /* punct */
  cbit_space, -1,          0,             /* space */
  cbit_word,  -1,          0,             /* word - a Perl extension */
  cbit_xdigit,-1,          0              /* xdigit */
};

/* Table of substitutes for \d etc when PCRE_UCP is set. The POSIX class
substitutes must be in the order of the names, defined above, and there are
both positive and negative cases. NULL means no substitute. */

#ifdef SUPPORT_UCP
static const pcre_uchar string_PNd[]  = {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_N, CHAR_d, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_pNd[]  = {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_N, CHAR_d, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_PXsp[] = {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_s, CHAR_p, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_pXsp[] = {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_s, CHAR_p, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_PXwd[] = {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_w, CHAR_d, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_pXwd[] = {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_w, CHAR_d, CHAR_RIGHT_CURLY_BRACKET, '\0' };

static const pcre_uchar *substitutes[] = {
  string_PNd,           /* \D */
  string_pNd,           /* \d */
  string_PXsp,          /* \S */       /* NOTE: Xsp is Perl space */
  string_pXsp,          /* \s */
  string_PXwd,          /* \W */
  string_pXwd           /* \w */
};

static const pcre_uchar string_pL[] =   {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_L, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_pLl[] =  {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_L, CHAR_l, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_pLu[] =  {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_L, CHAR_u, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_pXan[] = {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_a, CHAR_n, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_h[] =    {
  CHAR_BACKSLASH, CHAR_h, '\0' };
static const pcre_uchar string_pXps[] = {
  CHAR_BACKSLASH, CHAR_p, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_p, CHAR_s, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_PL[] =   {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_L, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_PLl[] =  {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_L, CHAR_l, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_PLu[] =  {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_L, CHAR_u, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_PXan[] = {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_a, CHAR_n, CHAR_RIGHT_CURLY_BRACKET, '\0' };
static const pcre_uchar string_H[] =    {
  CHAR_BACKSLASH, CHAR_H, '\0' };
static const pcre_uchar string_PXps[] = {
  CHAR_BACKSLASH, CHAR_P, CHAR_LEFT_CURLY_BRACKET,
  CHAR_X, CHAR_p, CHAR_s, CHAR_RIGHT_CURLY_BRACKET, '\0' };

static const pcre_uchar *posix_substitutes[] = {
  string_pL,            /* alpha */
  string_pLl,           /* lower */
  string_pLu,           /* upper */
  string_pXan,          /* alnum */
  NULL,                 /* ascii */
  string_h,             /* blank */
  NULL,                 /* cntrl */
  string_pNd,           /* digit */
  NULL,                 /* graph */
  NULL,                 /* print */
  NULL,                 /* punct */
  string_pXps,          /* space */    /* NOTE: Xps is POSIX space */
  string_pXwd,          /* word */
  NULL,                 /* xdigit */
  /* Negated cases */
  string_PL,            /* ^alpha */
  string_PLl,           /* ^lower */
  string_PLu,           /* ^upper */
  string_PXan,          /* ^alnum */
  NULL,                 /* ^ascii */
  string_H,             /* ^blank */
  NULL,                 /* ^cntrl */
  string_PNd,           /* ^digit */
  NULL,                 /* ^graph */
  NULL,                 /* ^print */
  NULL,                 /* ^punct */
  string_PXps,          /* ^space */   /* NOTE: Xps is POSIX space */
  string_PXwd,          /* ^word */
  NULL                  /* ^xdigit */
};
#define POSIX_SUBSIZE (sizeof(posix_substitutes) / sizeof(pcre_uchar *))
#endif

#define STRING(a)  # a
#define XSTRING(s) STRING(s)

/* The texts of compile-time error messages. These are "char *" because they
are passed to the outside world. Do not ever re-use any error number, because
they are documented. Always add a new error instead. Messages marked DEAD below
are no longer used. This used to be a table of strings, but in order to reduce
the number of relocations needed when a shared library is loaded dynamically,
it is now one long string. We cannot use a table of offsets, because the
lengths of inserts such as XSTRING(MAX_NAME_SIZE) are not known. Instead, we
simply count through to the one we want - this isn't a performance issue
because these strings are used only when there is a compilation error.

Each substring ends with \0 to insert a null character. This includes the final
substring, so that the whole string ends with \0\0, which can be detected when
counting through. */

static const char error_texts[] =
  "no error\0"
  "\\ at end of pattern\0"
  "\\c at end of pattern\0"
  "unrecognized character follows \\\0"
  "numbers out of order in {} quantifier\0"
  /* 5 */
  "number too big in {} quantifier\0"
  "missing terminating ] for character class\0"
  "invalid escape sequence in character class\0"
  "range out of order in character class\0"
  "nothing to repeat\0"
  /* 10 */
  "operand of unlimited repeat could match the empty string\0"  /** DEAD **/
  "internal error: unexpected repeat\0"
  "unrecognized character after (? or (?-\0"
  "POSIX named classes are supported only within a class\0"
  "missing )\0"
  /* 15 */
  "reference to non-existent subpattern\0"
  "erroffset passed as NULL\0"
  "unknown option bit(s) set\0"
  "missing ) after comment\0"
  "parentheses nested too deeply\0"  /** DEAD **/
  /* 20 */
  "regular expression is too large\0"
  "failed to get memory\0"
  "unmatched parentheses\0"
  "internal error: code overflow\0"
  "unrecognized character after (?<\0"
  /* 25 */
  "lookbehind assertion is not fixed length\0"
  "malformed number or name after (?(\0"
  "conditional group contains more than two branches\0"
  "assertion expected after (?(\0"
  "(?R or (?[+-]digits must be followed by )\0"
  /* 30 */
  "unknown POSIX class name\0"
  "POSIX collating elements are not supported\0"
  "this version of PCRE is compiled without UTF support\0"
  "spare error\0"  /** DEAD **/
  "character value in \\x{...} sequence is too large\0"
  /* 35 */
  "invalid condition (?(0)\0"
  "\\C not allowed in lookbehind assertion\0"
  "PCRE does not support \\L, \\l, \\N{name}, \\U, or \\u\0"
  "number after (?C is > 255\0"
  "closing ) for (?C expected\0"
  /* 40 */
  "recursive call could loop indefinitely\0"
  "unrecognized character after (?P\0"
  "syntax error in subpattern name (missing terminator)\0"
  "two named subpatterns have the same name\0"
  "invalid UTF-8 string\0"
  /* 45 */
  "support for \\P, \\p, and \\X has not been compiled\0"
  "malformed \\P or \\p sequence\0"
  "unknown property name after \\P or \\p\0"
  "subpattern name is too long (maximum " XSTRING(MAX_NAME_SIZE) " characters)\0"
  "too many named subpatterns (maximum " XSTRING(MAX_NAME_COUNT) ")\0"
  /* 50 */
  "repeated subpattern is too long\0"    /** DEAD **/
  "octal value is greater than \\377 in 8-bit non-UTF-8 mode\0"
  "internal error: overran compiling workspace\0"
  "internal error: previously-checked referenced subpattern not found\0"
  "DEFINE group contains more than one branch\0"
  /* 55 */
  "repeating a DEFINE group is not allowed\0"  /** DEAD **/
  "inconsistent NEWLINE options\0"
  "\\g is not followed by a braced, angle-bracketed, or quoted name/number or by a plain number\0"
  "a numbered reference must not be zero\0"
  "an argument is not allowed for (*ACCEPT), (*FAIL), or (*COMMIT)\0"
  /* 60 */
  "(*VERB) not recognized\0"
  "number is too big\0"
  "subpattern name expected\0"
  "digit expected after (?+\0"
  "] is an invalid data character in JavaScript compatibility mode\0"
  /* 65 */
  "different names for subpatterns of the same number are not allowed\0"
  "(*MARK) must have an argument\0"
  "this version of PCRE is not compiled with Unicode property support\0"
  "\\c must be followed by an ASCII character\0"
  "\\k is not followed by a braced, angle-bracketed, or quoted name\0"
  /* 70 */
  "internal error: unknown opcode in find_fixedlength()\0"
  "\\N is not supported in a class\0"
  "too many forward references\0"
  "disallowed Unicode code point (>= 0xd800 && <= 0xdfff)\0"
  "invalid UTF-16 string\0"
  ;

/* Table to identify digits and hex digits. This is used when compiling
patterns. Note that the tables in chartables are dependent on the locale, and
may mark arbitrary characters as digits - but the PCRE compiling code expects
to handle only 0-9, a-z, and A-Z as digits when compiling. That is why we have
a private table here. It costs 256 bytes, but it is a lot faster than doing
character value tests (at least in some simple cases I timed), and in some
applications one wants PCRE to compile efficiently as well as match
efficiently.

For convenience, we use the same bit definitions as in chartables:

  0x04   decimal digit
  0x08   hexadecimal digit

Then we can use ctype_digit and ctype_xdigit in the code. */

/* Using a simple comparison for decimal numbers rather than a memory read
is much faster, and the resulting code is simpler (the compiler turns it
into a subtraction and unsigned comparison). */

#define IS_DIGIT(x) ((x) >= CHAR_0 && (x) <= CHAR_9)

#ifndef EBCDIC

/* This is the "normal" case, for ASCII systems, and EBCDIC systems running in
UTF-8 mode. */

static const pcre_uint8 digitab[] =
  {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   0-  7 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   8- 15 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  16- 23 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*    - '  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  ( - /  */
  0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c, /*  0 - 7  */
  0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00, /*  8 - ?  */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /*  @ - G  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  H - O  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  P - W  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  X - _  */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /*  ` - g  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  h - o  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  p - w  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  x -127 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 128-135 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 136-143 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 144-151 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 152-159 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 160-167 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 168-175 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 176-183 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 184-191 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 192-199 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 200-207 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 208-215 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 216-223 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 224-231 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 232-239 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 240-247 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};/* 248-255 */

#else

/* This is the "abnormal" case, for EBCDIC systems not running in UTF-8 mode. */

static const pcre_uint8 digitab[] =
  {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   0-  7  0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*   8- 15    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  16- 23 10 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  32- 39 20 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  40- 47    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  48- 55 30 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  56- 63    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*    - 71 40 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  72- |     */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  & - 87 50 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  88- 95    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  - -103 60 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 104- ?     */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 112-119 70 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 120- "     */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /* 128- g  80 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  h -143    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 144- p  90 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  q -159    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 160- x  A0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  y -175    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  ^ -183 B0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 184-191    */
  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x00, /*  { - G  C0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  H -207    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  } - P  D0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  Q -223    */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  \ - X  E0 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  Y -239    */
  0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c, /*  0 - 7  F0 */
  0x0c,0x0c,0x00,0x00,0x00,0x00,0x00,0x00};/*  8 -255    */

static const pcre_uint8 ebcdic_chartab[] = { /* chartable partial dup */
  0x80,0x00,0x00,0x00,0x00,0x01,0x00,0x00, /*   0-  7 */
  0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x00, /*   8- 15 */
  0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00, /*  16- 23 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  24- 31 */
  0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00, /*  32- 39 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  40- 47 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  48- 55 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  56- 63 */
  0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*    - 71 */
  0x00,0x00,0x00,0x80,0x00,0x80,0x80,0x80, /*  72- |  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  & - 87 */
  0x00,0x00,0x00,0x80,0x80,0x80,0x00,0x00, /*  88- 95 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  - -103 */
  0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x80, /* 104- ?  */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 112-119 */
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* 120- "  */
  0x00,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /* 128- g  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  h -143 */
  0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /* 144- p  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  q -159 */
  0x00,0x00,0x12,0x12,0x12,0x12,0x12,0x12, /* 160- x  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  y -175 */
  0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /*  ^ -183 */
  0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00, /* 184-191 */
  0x80,0x1a,0x1a,0x1a,0x1a,0x1a,0x1a,0x12, /*  { - G  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  H -207 */
  0x00,0x12,0x12,0x12,0x12,0x12,0x12,0x12, /*  } - P  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  Q -223 */
  0x00,0x00,0x12,0x12,0x12,0x12,0x12,0x12, /*  \ - X  */
  0x12,0x12,0x00,0x00,0x00,0x00,0x00,0x00, /*  Y -239 */
  0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c, /*  0 - 7  */
  0x1c,0x1c,0x00,0x00,0x00,0x00,0x00,0x00};/*  8 -255 */
#endif


/* Definition to allow mutual recursion */

static BOOL
  compile_regex(int, pcre_uchar **, const pcre_uchar **, int *, BOOL, BOOL, int, int,
    int *, int *, branch_chain *, compile_data *, int *);



/*************************************************
*            Find an error text                  *
*************************************************/

/* The error texts are now all in one long string, to save on relocations. As
some of the text is of unknown length, we can't use a table of offsets.
Instead, just count through the strings. This is not a performance issue
because it happens only when there has been a compilation error.

Argument:   the error number
Returns:    pointer to the error string
*/

static const char *
find_error_text(int n)
{
const char *s = error_texts;
for (; n > 0; n--)
  {
  while (*s++ != 0) {};
  if (*s == 0) return "Error text not found (please report)";
  }
return s;
}


/*************************************************
*           Expand the workspace                 *
*************************************************/

/* This function is called during the second compiling phase, if the number of
forward references fills the existing workspace, which is originally a block on
the stack. A larger block is obtained from malloc() unless the ultimate limit
has been reached or the increase will be rather small.

Argument: pointer to the compile data block
Returns:  0 if all went well, else an error number
*/

static int
expand_workspace(compile_data *cd)
{
pcre_uchar *newspace;
int newsize = cd->workspace_size * 2;

if (newsize > COMPILE_WORK_SIZE_MAX) newsize = COMPILE_WORK_SIZE_MAX;
if (cd->workspace_size >= COMPILE_WORK_SIZE_MAX ||
    newsize - cd->workspace_size < WORK_SIZE_SAFETY_MARGIN)
 return ERR72;

newspace = (PUBL(malloc))(IN_UCHARS(newsize));
if (newspace == NULL) return ERR21;
memcpy(newspace, cd->start_workspace, cd->workspace_size * sizeof(pcre_uchar));
cd->hwm = (pcre_uchar *)newspace + (cd->hwm - cd->start_workspace);
if (cd->workspace_size > COMPILE_WORK_SIZE)
  (PUBL(free))((void *)cd->start_workspace);
cd->start_workspace = newspace;
cd->workspace_size = newsize;
return 0;
}



/*************************************************
*            Check for counted repeat            *
*************************************************/

/* This function is called when a '{' is encountered in a place where it might
start a quantifier. It looks ahead to see if it really is a quantifier or not.
It is only a quantifier if it is one of the forms {ddd} {ddd,} or {ddd,ddd}
where the ddds are digits.

Arguments:
  p         pointer to the first char after '{'

Returns:    TRUE or FALSE
*/

static BOOL
is_counted_repeat(const pcre_uchar *p)
{
if (!IS_DIGIT(*p)) return FALSE;
p++;
while (IS_DIGIT(*p)) p++;
if (*p == CHAR_RIGHT_CURLY_BRACKET) return TRUE;

if (*p++ != CHAR_COMMA) return FALSE;
if (*p == CHAR_RIGHT_CURLY_BRACKET) return TRUE;

if (!IS_DIGIT(*p)) return FALSE;
p++;
while (IS_DIGIT(*p)) p++;

return (*p == CHAR_RIGHT_CURLY_BRACKET);
}



/*************************************************
*            Handle escapes                      *
*************************************************/

/* This function is called when a \ has been encountered. It either returns a
positive value for a simple escape such as \n, or a negative value which
encodes one of the more complicated things such as \d. A backreference to group
n is returned as -(ESC_REF + n); ESC_REF is the highest ESC_xxx macro. When
UTF-8 is enabled, a positive value greater than 255 may be returned. On entry,
ptr is pointing at the \. On exit, it is on the final character of the escape
sequence.

Arguments:
  ptrptr         points to the pattern position pointer
  errorcodeptr   points to the errorcode variable
  bracount       number of previous extracting brackets
  options        the options bits
  isclass        TRUE if inside a character class

Returns:         zero or positive => a data character
                 negative => a special escape sequence
                 on error, errorcodeptr is set
*/

static int
check_escape(const pcre_uchar **ptrptr, int *errorcodeptr, int bracount,
  int options, BOOL isclass)
{
/* PCRE_UTF16 has the same value as PCRE_UTF8. */
BOOL utf = UTF_ENABLED((options & PCRE_UTF8) != 0);
const pcre_uchar *ptr = *ptrptr + 1;
pcre_int32 c;
int i;

GETCHARINCTEST(c, ptr);           /* Get character value, increment pointer */
ptr--;                            /* Set pointer back to the last byte */

/* If backslash is at the end of the pattern, it's an error. */

if (c == 0) *errorcodeptr = ERR1;

/* Non-alphanumerics are literals. For digits or letters, do an initial lookup
in a table. A non-zero result is something that can be returned immediately.
Otherwise further processing may be required. */

#ifndef EBCDIC  /* ASCII/UTF-8 coding */
/* Not alphanumeric */
else if (c < CHAR_0 || c > CHAR_z) {}
else if ((i = escapes[c - CHAR_0]) != 0) c = i;

#else           /* EBCDIC coding */
/* Not alphanumeric */
else if (c < 'a' || (!MAX_255(c) || (ebcdic_chartab[c] & 0x0E) == 0)) {}
else if ((i = escapes[c - 0x48]) != 0)  c = i;
#endif

/* Escapes that need further processing, or are illegal. */

else
  {
  const pcre_uchar *oldptr;
  BOOL braced, negated;

  switch (c)
    {
    /* A number of Perl escapes are not handled by PCRE. We give an explicit
    error. */

    case CHAR_l:
    case CHAR_L:
    *errorcodeptr = ERR37;
    break;

    case CHAR_u:
    if ((options & PCRE_JAVASCRIPT_COMPAT) != 0)
      {
      /* In JavaScript, \u must be followed by four hexadecimal numbers.
      Otherwise it is a lowercase u letter. */
      if (MAX_255(ptr[1]) && (digitab[ptr[1]] & ctype_xdigit) != 0
        && MAX_255(ptr[2]) && (digitab[ptr[2]] & ctype_xdigit) != 0
        && MAX_255(ptr[3]) && (digitab[ptr[3]] & ctype_xdigit) != 0
        && MAX_255(ptr[4]) && (digitab[ptr[4]] & ctype_xdigit) != 0)
        {
        c = 0;
        for (i = 0; i < 4; ++i)
          {
          register int cc = *(++ptr);
#ifndef EBCDIC  /* ASCII/UTF-8 coding */
          if (cc >= CHAR_a) cc -= 32;               /* Convert to upper case */
          c = (c << 4) + cc - ((cc < CHAR_A)? CHAR_0 : (CHAR_A - 10));
#else           /* EBCDIC coding */
          if (cc >= CHAR_a && cc <= CHAR_z) cc += 64;  /* Convert to upper case */
          c = (c << 4) + cc - ((cc >= CHAR_0)? CHAR_0 : (CHAR_A - 10));
#endif
          }
        }
      }
    else
      *errorcodeptr = ERR37;
    break;

    case CHAR_U:
    /* In JavaScript, \U is an uppercase U letter. */
    if ((options & PCRE_JAVASCRIPT_COMPAT) == 0) *errorcodeptr = ERR37;
    break;

    /* In a character class, \g is just a literal "g". Outside a character
    class, \g must be followed by one of a number of specific things:

    (1) A number, either plain or braced. If positive, it is an absolute
    backreference. If negative, it is a relative backreference. This is a Perl
    5.10 feature.

    (2) Perl 5.10 also supports \g{name} as a reference to a named group. This
    is part of Perl's movement towards a unified syntax for back references. As
    this is synonymous with \k{name}, we fudge it up by pretending it really
    was \k.

    (3) For Oniguruma compatibility we also support \g followed by a name or a
    number either in angle brackets or in single quotes. However, these are
    (possibly recursive) subroutine calls, _not_ backreferences. Just return
    the -ESC_g code (cf \k). */

    case CHAR_g:
    if (isclass) break;
    if (ptr[1] == CHAR_LESS_THAN_SIGN || ptr[1] == CHAR_APOSTROPHE)
      {
      c = -ESC_g;
      break;
      }

    /* Handle the Perl-compatible cases */

    if (ptr[1] == CHAR_LEFT_CURLY_BRACKET)
      {
      const pcre_uchar *p;
      for (p = ptr+2; *p != 0 && *p != CHAR_RIGHT_CURLY_BRACKET; p++)
        if (*p != CHAR_MINUS && !IS_DIGIT(*p)) break;
      if (*p != 0 && *p != CHAR_RIGHT_CURLY_BRACKET)
        {
        c = -ESC_k;
        break;
        }
      braced = TRUE;
      ptr++;
      }
    else braced = FALSE;

    if (ptr[1] == CHAR_MINUS)
      {
      negated = TRUE;
      ptr++;
      }
    else negated = FALSE;

    /* The integer range is limited by the machine's int representation. */
    c = 0;
    while (IS_DIGIT(ptr[1]))
      {
      if (((unsigned int)c) > INT_MAX / 10) /* Integer overflow */
        {
        c = -1;
        break;
        }
      c = c * 10 + *(++ptr) - CHAR_0;
      }
    if (((unsigned int)c) > INT_MAX) /* Integer overflow */
      {
      while (IS_DIGIT(ptr[1]))
        ptr++;
      *errorcodeptr = ERR61;
      break;
      }

    if (braced && *(++ptr) != CHAR_RIGHT_CURLY_BRACKET)
      {
      *errorcodeptr = ERR57;
      break;
      }

    if (c == 0)
      {
      *errorcodeptr = ERR58;
      break;
      }

    if (negated)
      {
      if (c > bracount)
        {
        *errorcodeptr = ERR15;
        break;
        }
      c = bracount - (c - 1);
      }

    c = -(ESC_REF + c);
    break;

    /* The handling of escape sequences consisting of a string of digits
    starting with one that is not zero is not straightforward. By experiment,
    the way Perl works seems to be as follows:

    Outside a character class, the digits are read as a decimal number. If the
    number is less than 10, or if there are that many previous extracting
    left brackets, then it is a back reference. Otherwise, up to three octal
    digits are read to form an escaped byte. Thus \123 is likely to be octal
    123 (cf \0123, which is octal 012 followed by the literal 3). If the octal
    value is greater than 377, the least significant 8 bits are taken. Inside a
    character class, \ followed by a digit is always an octal number. */

    case CHAR_1: case CHAR_2: case CHAR_3: case CHAR_4: case CHAR_5:
    case CHAR_6: case CHAR_7: case CHAR_8: case CHAR_9:

    if (!isclass)
      {
      oldptr = ptr;
      /* The integer range is limited by the machine's int representation. */
      c -= CHAR_0;
      while (IS_DIGIT(ptr[1]))
        {
        if (((unsigned int)c) > INT_MAX / 10) /* Integer overflow */
          {
          c = -1;
          break;
          }
        c = c * 10 + *(++ptr) - CHAR_0;
        }
      if (((unsigned int)c) > INT_MAX) /* Integer overflow */
        {
        while (IS_DIGIT(ptr[1]))
          ptr++;
        *errorcodeptr = ERR61;
        break;
        }
      if (c < 10 || c <= bracount)
        {
        c = -(ESC_REF + c);
        break;
        }
      ptr = oldptr;      /* Put the pointer back and fall through */
      }

    /* Handle an octal number following \. If the first digit is 8 or 9, Perl
    generates a binary zero byte and treats the digit as a following literal.
    Thus we have to pull back the pointer by one. */

    if ((c = *ptr) >= CHAR_8)
      {
      ptr--;
      c = 0;
      break;
      }

    /* \0 always starts an octal number, but we may drop through to here with a
    larger first octal digit. The original code used just to take the least
    significant 8 bits of octal numbers (I think this is what early Perls used
    to do). Nowadays we allow for larger numbers in UTF-8 mode and 16-bit mode,
    but no more than 3 octal digits. */

    case CHAR_0:
    c -= CHAR_0;
    while(i++ < 2 && ptr[1] >= CHAR_0 && ptr[1] <= CHAR_7)
        c = c * 8 + *(++ptr) - CHAR_0;
#ifdef COMPILE_PCRE8
    if (!utf && c > 0xff) *errorcodeptr = ERR51;
#endif
    break;

    /* \x is complicated. \x{ddd} is a character number which can be greater
    than 0xff in utf or non-8bit mode, but only if the ddd are hex digits.
    If not, { is treated as a data character. */

    case CHAR_x:
    if ((options & PCRE_JAVASCRIPT_COMPAT) != 0)
      {
      /* In JavaScript, \x must be followed by two hexadecimal numbers.
      Otherwise it is a lowercase x letter. */
      if (MAX_255(ptr[1]) && (digitab[ptr[1]] & ctype_xdigit) != 0
        && MAX_255(ptr[2]) && (digitab[ptr[2]] & ctype_xdigit) != 0)
        {
        c = 0;
        for (i = 0; i < 2; ++i)
          {
          register int cc = *(++ptr);
#ifndef EBCDIC  /* ASCII/UTF-8 coding */
          if (cc >= CHAR_a) cc -= 32;               /* Convert to upper case */
          c = (c << 4) + cc - ((cc < CHAR_A)? CHAR_0 : (CHAR_A - 10));
#else           /* EBCDIC coding */
          if (cc >= CHAR_a && cc <= CHAR_z) cc += 64;  /* Convert to upper case */
          c = (c << 4) + cc - ((cc >= CHAR_0)? CHAR_0 : (CHAR_A - 10));
#endif
          }
        }
      break;
      }

    if (ptr[1] == CHAR_LEFT_CURLY_BRACKET)
      {
      const pcre_uchar *pt = ptr + 2;

      c = 0;
      while (MAX_255(*pt) && (digitab[*pt] & ctype_xdigit) != 0)
        {
        register int cc = *pt++;
        if (c == 0 && cc == CHAR_0) continue;     /* Leading zeroes */

#ifndef EBCDIC  /* ASCII/UTF-8 coding */
        if (cc >= CHAR_a) cc -= 32;               /* Convert to upper case */
        c = (c << 4) + cc - ((cc < CHAR_A)? CHAR_0 : (CHAR_A - 10));
#else           /* EBCDIC coding */
        if (cc >= CHAR_a && cc <= CHAR_z) cc += 64;  /* Convert to upper case */
        c = (c << 4) + cc - ((cc >= CHAR_0)? CHAR_0 : (CHAR_A - 10));
#endif

#ifdef COMPILE_PCRE8
        if (c > (utf ? 0x10ffff : 0xff)) { c = -1; break; }
#else
#ifdef COMPILE_PCRE16
        if (c > (utf ? 0x10ffff : 0xffff)) { c = -1; break; }
#endif
#endif
        }

      if (c < 0)
        {
        while (MAX_255(*pt) && (digitab[*pt] & ctype_xdigit) != 0) pt++;
        *errorcodeptr = ERR34;
        }

      if (*pt == CHAR_RIGHT_CURLY_BRACKET)
        {
        if (utf && c >= 0xd800 && c <= 0xdfff) *errorcodeptr = ERR73;
        ptr = pt;
        break;
        }

      /* If the sequence of hex digits does not end with '}', then we don't
      recognize this construct; fall through to the normal \x handling. */
      }

    /* Read just a single-byte hex-defined char */

    c = 0;
    while (i++ < 2 && MAX_255(ptr[1]) && (digitab[ptr[1]] & ctype_xdigit) != 0)
      {
      int cc;                                  /* Some compilers don't like */
      cc = *(++ptr);                           /* ++ in initializers */
#ifndef EBCDIC  /* ASCII/UTF-8 coding */
      if (cc >= CHAR_a) cc -= 32;              /* Convert to upper case */
      c = c * 16 + cc - ((cc < CHAR_A)? CHAR_0 : (CHAR_A - 10));
#else           /* EBCDIC coding */
      if (cc <= CHAR_z) cc += 64;              /* Convert to upper case */
      c = c * 16 + cc - ((cc >= CHAR_0)? CHAR_0 : (CHAR_A - 10));
#endif
      }
    break;

    /* For \c, a following letter is upper-cased; then the 0x40 bit is flipped.
    An error is given if the byte following \c is not an ASCII character. This
    coding is ASCII-specific, but then the whole concept of \cx is
    ASCII-specific. (However, an EBCDIC equivalent has now been added.) */

    case CHAR_c:
    c = *(++ptr);
    if (c == 0)
      {
      *errorcodeptr = ERR2;
      break;
      }
#ifndef EBCDIC    /* ASCII/UTF-8 coding */
    if (c > 127)  /* Excludes all non-ASCII in either mode */
      {
      *errorcodeptr = ERR68;
      break;
      }
    if (c >= CHAR_a && c <= CHAR_z) c -= 32;
    c ^= 0x40;
#else             /* EBCDIC coding */
    if (c >= CHAR_a && c <= CHAR_z) c += 64;
    c ^= 0xC0;
#endif
    break;

    /* PCRE_EXTRA enables extensions to Perl in the matter of escapes. Any
    other alphanumeric following \ is an error if PCRE_EXTRA was set;
    otherwise, for Perl compatibility, it is a literal. This code looks a bit
    odd, but there used to be some cases other than the default, and there may
    be again in future, so I haven't "optimized" it. */

    default:
    if ((options & PCRE_EXTRA) != 0) switch(c)
      {
      default:
      *errorcodeptr = ERR3;
      break;
      }
    break;
    }
  }

/* Perl supports \N{name} for character names, as well as plain \N for "not
newline". PCRE does not support \N{name}. However, it does support
quantification such as \N{2,3}. */

if (c == -ESC_N && ptr[1] == CHAR_LEFT_CURLY_BRACKET &&
     !is_counted_repeat(ptr+2))
  *errorcodeptr = ERR37;

/* If PCRE_UCP is set, we change the values for \d etc. */

if ((options & PCRE_UCP) != 0 && c <= -ESC_D && c >= -ESC_w)
  c -= (ESC_DU - ESC_D);

/* Set the pointer to the final character before returning. */

*ptrptr = ptr;
return c;
}



#ifdef SUPPORT_UCP
/*************************************************
*               Handle \P and \p                 *
*************************************************/

/* This function is called after \P or \p has been encountered, provided that
PCRE is compiled with support for Unicode properties. On entry, ptrptr is
pointing at the P or p. On exit, it is pointing at the final character of the
escape sequence.

Argument:
  ptrptr         points to the pattern position pointer
  negptr         points to a boolean that is set TRUE for negation else FALSE
  dptr           points to an int that is set to the detailed property value
  errorcodeptr   points to the error code variable

Returns:         type value from ucp_type_table, or -1 for an invalid type
*/

static int
get_ucp(const pcre_uchar **ptrptr, BOOL *negptr, int *dptr, int *errorcodeptr)
{
int c, i, bot, top;
const pcre_uchar *ptr = *ptrptr;
pcre_uchar name[32];

c = *(++ptr);
if (c == 0) goto ERROR_RETURN;

*negptr = FALSE;

/* \P or \p can be followed by a name in {}, optionally preceded by ^ for
negation. */

if (c == CHAR_LEFT_CURLY_BRACKET)
  {
  if (ptr[1] == CHAR_CIRCUMFLEX_ACCENT)
    {
    *negptr = TRUE;
    ptr++;
    }
  for (i = 0; i < (int)(sizeof(name) / sizeof(pcre_uchar)) - 1; i++)
    {
    c = *(++ptr);
    if (c == 0) goto ERROR_RETURN;
    if (c == CHAR_RIGHT_CURLY_BRACKET) break;
    name[i] = c;
    }
  if (c != CHAR_RIGHT_CURLY_BRACKET) goto ERROR_RETURN;
  name[i] = 0;
  }

/* Otherwise there is just one following character */

else
  {
  name[0] = c;
  name[1] = 0;
  }

*ptrptr = ptr;

/* Search for a recognized property name using binary chop */

bot = 0;
top = PRIV(utt_size);

while (bot < top)
  {
  i = (bot + top) >> 1;
  c = STRCMP_UC_C8(name, PRIV(utt_names) + PRIV(utt)[i].name_offset);
  if (c == 0)
    {
    *dptr = PRIV(utt)[i].value;
    return PRIV(utt)[i].type;
    }
  if (c > 0) bot = i + 1; else top = i;
  }

*errorcodeptr = ERR47;
*ptrptr = ptr;
return -1;

ERROR_RETURN:
*errorcodeptr = ERR46;
*ptrptr = ptr;
return -1;
}
#endif




/*************************************************
*         Read repeat counts                     *
*************************************************/

/* Read an item of the form {n,m} and return the values. This is called only
after is_counted_repeat() has confirmed that a repeat-count quantifier exists,
so the syntax is guaranteed to be correct, but we need to check the values.

Arguments:
  p              pointer to first char after '{'
  minp           pointer to int for min
  maxp           pointer to int for max
                 returned as -1 if no max
  errorcodeptr   points to error code variable

Returns:         pointer to '}' on success;
                 current ptr on error, with errorcodeptr set non-zero
*/

static const pcre_uchar *
read_repeat_counts(const pcre_uchar *p, int *minp, int *maxp, int *errorcodeptr)
{
int min = 0;
int max = -1;

/* Read the minimum value and do a paranoid check: a negative value indicates
an integer overflow. */

while (IS_DIGIT(*p)) min = min * 10 + *p++ - CHAR_0;
if (min < 0 || min > 65535)
  {
  *errorcodeptr = ERR5;
  return p;
  }

/* Read the maximum value if there is one, and again do a paranoid on its size.
Also, max must not be less than min. */

if (*p == CHAR_RIGHT_CURLY_BRACKET) max = min; else
  {
  if (*(++p) != CHAR_RIGHT_CURLY_BRACKET)
    {
    max = 0;
    while(IS_DIGIT(*p)) max = max * 10 + *p++ - CHAR_0;
    if (max < 0 || max > 65535)
      {
      *errorcodeptr = ERR5;
      return p;
      }
    if (max < min)
      {
      *errorcodeptr = ERR4;
      return p;
      }
    }
  }

/* Fill in the required variables, and pass back the pointer to the terminating
'}'. */

*minp = min;
*maxp = max;
return p;
}



/*************************************************
*  Subroutine for finding forward reference      *
*************************************************/

/* This recursive function is called only from find_parens() below. The
top-level call starts at the beginning of the pattern. All other calls must
start at a parenthesis. It scans along a pattern's text looking for capturing
subpatterns, and counting them. If it finds a named pattern that matches the
name it is given, it returns its number. Alternatively, if the name is NULL, it
returns when it reaches a given numbered subpattern. Recursion is used to keep
track of subpatterns that reset the capturing group numbers - the (?| feature.

This function was originally called only from the second pass, in which we know
that if (?< or (?' or (?P< is encountered, the name will be correctly
terminated because that is checked in the first pass. There is now one call to
this function in the first pass, to check for a recursive back reference by
name (so that we can make the whole group atomic). In this case, we need check
only up to the current position in the pattern, and that is still OK because
and previous occurrences will have been checked. To make this work, the test
for "end of pattern" is a check against cd->end_pattern in the main loop,
instead of looking for a binary zero. This means that the special first-pass
call can adjust cd->end_pattern temporarily. (Checks for binary zero while
processing items within the loop are OK, because afterwards the main loop will
terminate.)

Arguments:
  ptrptr       address of the current character pointer (updated)
  cd           compile background data
  name         name to seek, or NULL if seeking a numbered subpattern
  lorn         name length, or subpattern number if name is NULL
  xmode        TRUE if we are in /x mode
  utf          TRUE if we are in UTF-8 / UTF-16 mode
  count        pointer to the current capturing subpattern number (updated)

Returns:       the number of the named subpattern, or -1 if not found
*/

static int
find_parens_sub(pcre_uchar **ptrptr, compile_data *cd, const pcre_uchar *name, int lorn,
  BOOL xmode, BOOL utf, int *count)
{
pcre_uchar *ptr = *ptrptr;
int start_count = *count;
int hwm_count = start_count;
BOOL dup_parens = FALSE;

/* If the first character is a parenthesis, check on the type of group we are
dealing with. The very first call may not start with a parenthesis. */

if (ptr[0] == CHAR_LEFT_PARENTHESIS)
  {
  /* Handle specials such as (*SKIP) or (*UTF8) etc. */

  if (ptr[1] == CHAR_ASTERISK) ptr += 2;

  /* Handle a normal, unnamed capturing parenthesis. */

  else if (ptr[1] != CHAR_QUESTION_MARK)
    {
    *count += 1;
    if (name == NULL && *count == lorn) return *count;
    ptr++;
    }

  /* All cases now have (? at the start. Remember when we are in a group
  where the parenthesis numbers are duplicated. */

  else if (ptr[2] == CHAR_VERTICAL_LINE)
    {
    ptr += 3;
    dup_parens = TRUE;
    }

  /* Handle comments; all characters are allowed until a ket is reached. */

  else if (ptr[2] == CHAR_NUMBER_SIGN)
    {
    for (ptr += 3; *ptr != 0; ptr++) if (*ptr == CHAR_RIGHT_PARENTHESIS) break;
    goto FAIL_EXIT;
    }

  /* Handle a condition. If it is an assertion, just carry on so that it
  is processed as normal. If not, skip to the closing parenthesis of the
  condition (there can't be any nested parens). */

  else if (ptr[2] == CHAR_LEFT_PARENTHESIS)
    {
    ptr += 2;
    if (ptr[1] != CHAR_QUESTION_MARK)
      {
      while (*ptr != 0 && *ptr != CHAR_RIGHT_PARENTHESIS) ptr++;
      if (*ptr != 0) ptr++;
      }
    }

  /* Start with (? but not a condition. */

  else
    {
    ptr += 2;
    if (*ptr == CHAR_P) ptr++;                      /* Allow optional P */

    /* We have to disambiguate (?<! and (?<= from (?<name> for named groups */

    if ((*ptr == CHAR_LESS_THAN_SIGN && ptr[1] != CHAR_EXCLAMATION_MARK &&
        ptr[1] != CHAR_EQUALS_SIGN) || *ptr == CHAR_APOSTROPHE)
      {
      int term;
      const pcre_uchar *thisname;
      *count += 1;
      if (name == NULL && *count == lorn) return *count;
      term = *ptr++;
      if (term == CHAR_LESS_THAN_SIGN) term = CHAR_GREATER_THAN_SIGN;
      thisname = ptr;
      while (*ptr != term) ptr++;
      if (name != NULL && lorn == ptr - thisname &&
          STRNCMP_UC_UC(name, thisname, lorn) == 0)
        return *count;
      term++;
      }
    }
  }

/* Past any initial parenthesis handling, scan for parentheses or vertical
bars. Stop if we get to cd->end_pattern. Note that this is important for the
first-pass call when this value is temporarily adjusted to stop at the current
position. So DO NOT change this to a test for binary zero. */

for (; ptr < cd->end_pattern; ptr++)
  {
  /* Skip over backslashed characters and also entire \Q...\E */

  if (*ptr == CHAR_BACKSLASH)
    {
    if (*(++ptr) == 0) goto FAIL_EXIT;
    if (*ptr == CHAR_Q) for (;;)
      {
      while (*(++ptr) != 0 && *ptr != CHAR_BACKSLASH) {};
      if (*ptr == 0) goto FAIL_EXIT;
      if (*(++ptr) == CHAR_E) break;
      }
    continue;
    }

  /* Skip over character classes; this logic must be similar to the way they
  are handled for real. If the first character is '^', skip it. Also, if the
  first few characters (either before or after ^) are \Q\E or \E we skip them
  too. This makes for compatibility with Perl. Note the use of STR macros to
  encode "Q\\E" so that it works in UTF-8 on EBCDIC platforms. */

  if (*ptr == CHAR_LEFT_SQUARE_BRACKET)
    {
    BOOL negate_class = FALSE;
    for (;;)
      {
      if (ptr[1] == CHAR_BACKSLASH)
        {
        if (ptr[2] == CHAR_E)
          ptr+= 2;
        else if (STRNCMP_UC_C8(ptr + 2,
                 STR_Q STR_BACKSLASH STR_E, 3) == 0)
          ptr += 4;
        else
          break;
        }
      else if (!negate_class && ptr[1] == CHAR_CIRCUMFLEX_ACCENT)
        {
        negate_class = TRUE;
        ptr++;
        }
      else break;
      }

    /* If the next character is ']', it is a data character that must be
    skipped, except in JavaScript compatibility mode. */

    if (ptr[1] == CHAR_RIGHT_SQUARE_BRACKET &&
        (cd->external_options & PCRE_JAVASCRIPT_COMPAT) == 0)
      ptr++;

    while (*(++ptr) != CHAR_RIGHT_SQUARE_BRACKET)
      {
      if (*ptr == 0) return -1;
      if (*ptr == CHAR_BACKSLASH)
        {
        if (*(++ptr) == 0) goto FAIL_EXIT;
        if (*ptr == CHAR_Q) for (;;)
          {
          while (*(++ptr) != 0 && *ptr != CHAR_BACKSLASH) {};
          if (*ptr == 0) goto FAIL_EXIT;
          if (*(++ptr) == CHAR_E) break;
          }
        continue;
        }
      }
    continue;
    }

  /* Skip comments in /x mode */

  if (xmode && *ptr == CHAR_NUMBER_SIGN)
    {
    ptr++;
    while (*ptr != 0)
      {
      if (IS_NEWLINE(ptr)) { ptr += cd->nllen - 1; break; }
      ptr++;
#ifdef SUPPORT_UTF
      if (utf) FORWARDCHAR(ptr);
#endif
      }
    if (*ptr == 0) goto FAIL_EXIT;
    continue;
    }

  /* Check for the special metacharacters */

  if (*ptr == CHAR_LEFT_PARENTHESIS)
    {
    int rc = find_parens_sub(&ptr, cd, name, lorn, xmode, utf, count);
    if (rc > 0) return rc;
    if (*ptr == 0) goto FAIL_EXIT;
    }

  else if (*ptr == CHAR_RIGHT_PARENTHESIS)
    {
    if (dup_parens && *count < hwm_count) *count = hwm_count;
    goto FAIL_EXIT;
    }

  else if (*ptr == CHAR_VERTICAL_LINE && dup_parens)
    {
    if (*count > hwm_count) hwm_count = *count;
    *count = start_count;
    }
  }

FAIL_EXIT:
*ptrptr = ptr;
return -1;
}




/*************************************************
*       Find forward referenced subpattern       *
*************************************************/

/* This function scans along a pattern's text looking for capturing
subpatterns, and counting them. If it finds a named pattern that matches the
name it is given, it returns its number. Alternatively, if the name is NULL, it
returns when it reaches a given numbered subpattern. This is used for forward
references to subpatterns. We used to be able to start this scan from the
current compiling point, using the current count value from cd->bracount, and
do it all in a single loop, but the addition of the possibility of duplicate
subpattern numbers means that we have to scan from the very start, in order to
take account of such duplicates, and to use a recursive function to keep track
of the different types of group.

Arguments:
  cd           compile background data
  name         name to seek, or NULL if seeking a numbered subpattern
  lorn         name length, or subpattern number if name is NULL
  xmode        TRUE if we are in /x mode
  utf          TRUE if we are in UTF-8 / UTF-16 mode

Returns:       the number of the found subpattern, or -1 if not found
*/

static int
find_parens(compile_data *cd, const pcre_uchar *name, int lorn, BOOL xmode,
  BOOL utf)
{
pcre_uchar *ptr = (pcre_uchar *)cd->start_pattern;
int count = 0;
int rc;

/* If the pattern does not start with an opening parenthesis, the first call
to find_parens_sub() will scan right to the end (if necessary). However, if it
does start with a parenthesis, find_parens_sub() will return when it hits the
matching closing parens. That is why we have to have a loop. */

for (;;)
  {
  rc = find_parens_sub(&ptr, cd, name, lorn, xmode, utf, &count);
  if (rc > 0 || *ptr++ == 0) break;
  }

return rc;
}




/*************************************************
*      Find first significant op code            *
*************************************************/

/* This is called by several functions that scan a compiled expression looking
for a fixed first character, or an anchoring op code etc. It skips over things
that do not influence this. For some calls, it makes sense to skip negative
forward and all backward assertions, and also the \b assertion; for others it
does not.

Arguments:
  code         pointer to the start of the group
  skipassert   TRUE if certain assertions are to be skipped

Returns:       pointer to the first significant opcode
*/

static const pcre_uchar*
first_significant_code(const pcre_uchar *code, BOOL skipassert)
{
for (;;)
  {
  switch ((int)*code)
    {
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    if (!skipassert) return code;
    do code += GET(code, 1); while (*code == OP_ALT);
    code += PRIV(OP_lengths)[*code];
    break;

    case OP_WORD_BOUNDARY:
    case OP_NOT_WORD_BOUNDARY:
    if (!skipassert) return code;
    /* Fall through */

    case OP_CALLOUT:
    case OP_CREF:
    case OP_NCREF:
    case OP_RREF:
    case OP_NRREF:
    case OP_DEF:
    code += PRIV(OP_lengths)[*code];
    break;

    default:
    return code;
    }
  }
/* Control never reaches here */
}




/*************************************************
*        Find the fixed length of a branch       *
*************************************************/

/* Scan a branch and compute the fixed length of subject that will match it,
if the length is fixed. This is needed for dealing with backward assertions.
In UTF8 mode, the result is in characters rather than bytes. The branch is
temporarily terminated with OP_END when this function is called.

This function is called when a backward assertion is encountered, so that if it
fails, the error message can point to the correct place in the pattern.
However, we cannot do this when the assertion contains subroutine calls,
because they can be forward references. We solve this by remembering this case
and doing the check at the end; a flag specifies which mode we are running in.

Arguments:
  code     points to the start of the pattern (the bracket)
  utf      TRUE in UTF-8 / UTF-16 mode
  atend    TRUE if called when the pattern is complete
  cd       the "compile data" structure

Returns:   the fixed length,
             or -1 if there is no fixed length,
             or -2 if \C was encountered (in UTF-8 mode only)
             or -3 if an OP_RECURSE item was encountered and atend is FALSE
             or -4 if an unknown opcode was encountered (internal error)
*/

static int
find_fixedlength(pcre_uchar *code, BOOL utf, BOOL atend, compile_data *cd)
{
int length = -1;

register int branchlength = 0;
register pcre_uchar *cc = code + 1 + LINK_SIZE;

/* Scan along the opcodes for this branch. If we get to the end of the
branch, check the length against that of the other branches. */

for (;;)
  {
  int d;
  pcre_uchar *ce, *cs;
  register int op = *cc;

  switch (op)
    {
    /* We only need to continue for OP_CBRA (normal capturing bracket) and
    OP_BRA (normal non-capturing bracket) because the other variants of these
    opcodes are all concerned with unlimited repeated groups, which of course
    are not of fixed length. */

    case OP_CBRA:
    case OP_BRA:
    case OP_ONCE:
    case OP_ONCE_NC:
    case OP_COND:
    d = find_fixedlength(cc + ((op == OP_CBRA)? IMM2_SIZE : 0), utf, atend, cd);
    if (d < 0) return d;
    branchlength += d;
    do cc += GET(cc, 1); while (*cc == OP_ALT);
    cc += 1 + LINK_SIZE;
    break;

    /* Reached end of a branch; if it's a ket it is the end of a nested call.
    If it's ALT it is an alternation in a nested call. An ACCEPT is effectively
    an ALT. If it is END it's the end of the outer call. All can be handled by
    the same code. Note that we must not include the OP_KETRxxx opcodes here,
    because they all imply an unlimited repeat. */

    case OP_ALT:
    case OP_KET:
    case OP_END:
    case OP_ACCEPT:
    case OP_ASSERT_ACCEPT:
    if (length < 0) length = branchlength;
      else if (length != branchlength) return -1;
    if (*cc != OP_ALT) return length;
    cc += 1 + LINK_SIZE;
    branchlength = 0;
    break;

    /* A true recursion implies not fixed length, but a subroutine call may
    be OK. If the subroutine is a forward reference, we can't deal with
    it until the end of the pattern, so return -3. */

    case OP_RECURSE:
    if (!atend) return -3;
    cs = ce = (pcre_uchar *)cd->start_code + GET(cc, 1);  /* Start subpattern */
    do ce += GET(ce, 1); while (*ce == OP_ALT);           /* End subpattern */
    if (cc > cs && cc < ce) return -1;                    /* Recursion */
    d = find_fixedlength(cs + IMM2_SIZE, utf, atend, cd);
    if (d < 0) return d;
    branchlength += d;
    cc += 1 + LINK_SIZE;
    break;

    /* Skip over assertive subpatterns */

    case OP_ASSERT:
    case OP_ASSERT_NOT:
    case OP_ASSERTBACK:
    case OP_ASSERTBACK_NOT:
    do cc += GET(cc, 1); while (*cc == OP_ALT);
    cc += PRIV(OP_lengths)[*cc];
    break;

    /* Skip over things that don't match chars */

    case OP_MARK:
    case OP_PRUNE_ARG:
    case OP_SKIP_ARG:
    case OP_THEN_ARG:
    cc += cc[1] + PRIV(OP_lengths)[*cc];
    break;

    case OP_CALLOUT:
    case OP_CIRC:
    case OP_CIRCM:
    case OP_CLOSE:
    case OP_COMMIT:
    case OP_CREF:
    case OP_DEF:
    case OP_DOLL:
    case OP_DOLLM:
    case OP_EOD:
    case OP_EODN:
    case OP_FAIL:
    case OP_NCREF:
    case OP_NRREF:
    case OP_NOT_WORD_BOUNDARY:
    case OP_PRUNE:
    case OP_REVERSE:
    case OP_RREF:
    case OP_SET_SOM:
    case OP_SKIP:
    case OP_SOD:
    case OP_SOM:
    case OP_THEN:
    case OP_WORD_BOUNDARY:
    cc += PRIV(OP_lengths)[*cc];
    break;

    /* Handle literal characters */

    case OP_CHAR:
    case OP_CHARI:
    case OP_NOT:
    case OP_NOTI:
    branchlength++;
    cc += 2;
#ifdef SUPPORT_UTF
    if (utf && HAS_EXTRALEN(cc[-1])) cc += GET_EXTRALEN(cc[-1]);
#endif
    break;

    /* Handle exact repetitions. The count is already in characters, but we
    need to skip over a multibyte character in UTF8 mode.  */

    case OP_EXACT:
    case OP_EXACTI:
    case OP_NOTEXACT:
    case OP_NOTEXACTI:
    branchlength += GET2(cc,1);
    cc += 2 + IMM2_SIZE;
#ifdef SUPPORT_UTF
    if (utf && HAS_EXTRALEN(cc[-1])) cc += GET_EXTRALEN(cc[-1]);
#endif
    break;

    case OP_TYPEEXACT:
    branchlength += GET2(cc,1);
    if (cc[1 + IMM2_SIZE] == OP_PROP || cc[1 + IMM2_SIZE] == OP_NOTPROP) cc += 2;
    cc += 1 + IMM2_SIZE + 1;
    break;

    /* Handle single-char matchers */

    case OP_PROP:
    case OP_NOTPROP:
    cc += 2;
    /* Fall through */

    case OP_HSPACE:
    case OP_VSPACE:
    case OP_NOT_HSPACE:
    case OP_NOT_VSPACE:
    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
    branchlength++;
    cc++;
    break;

    /* The single-byte matcher isn't allowed. This only happens in UTF-8 mode;
    otherwise \C is coded as OP_ALLANY. */

    case OP_ANYBYTE:
    return -2;

    /* Check a class for variable quantification */

#if defined SUPPORT_UTF || defined COMPILE_PCRE16
    case OP_XCLASS:
    cc += GET(cc, 1) - PRIV(OP_lengths)[OP_CLASS];
    /* Fall through */
#endif

    case OP_CLASS:
    case OP_NCLASS:
    cc += PRIV(OP_lengths)[OP_CLASS];

    switch (*cc)
      {
      case OP_CRPLUS:
      case OP_CRMINPLUS:
      case OP_CRSTAR:
      case OP_CRMINSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      return -1;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      if (GET2(cc,1) != GET2(cc,1+IMM2_SIZE)) return -1;
      branchlength += GET2(cc,1);
      cc += 1 + 2 * IMM2_SIZE;
      break;

      default:
      branchlength++;
      }
    break;

    /* Anything else is variable length */

    case OP_ANYNL:
    case OP_BRAMINZERO:
    case OP_BRAPOS:
    case OP_BRAPOSZERO:
    case OP_BRAZERO:
    case OP_CBRAPOS:
    case OP_EXTUNI:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_KETRPOS:
    case OP_MINPLUS:
    case OP_MINPLUSI:
    case OP_MINQUERY:
    case OP_MINQUERYI:
    case OP_MINSTAR:
    case OP_MINSTARI:
    case OP_MINUPTO:
    case OP_MINUPTOI:
    case OP_NOTMINPLUS:
    case OP_NOTMINPLUSI:
    case OP_NOTMINQUERY:
    case OP_NOTMINQUERYI:
    case OP_NOTMINSTAR:
    case OP_NOTMINSTARI:
    case OP_NOTMINUPTO:
    case OP_NOTMINUPTOI:
    case OP_NOTPLUS:
    case OP_NOTPLUSI:
    case OP_NOTPOSPLUS:
    case OP_NOTPOSPLUSI:
    case OP_NOTPOSQUERY:
    case OP_NOTPOSQUERYI:
    case OP_NOTPOSSTAR:
    case OP_NOTPOSSTARI:
    case OP_NOTPOSUPTO:
    case OP_NOTPOSUPTOI:
    case OP_NOTQUERY:
    case OP_NOTQUERYI:
    case OP_NOTSTAR:
    case OP_NOTSTARI:
    case OP_NOTUPTO:
    case OP_NOTUPTOI:
    case OP_PLUS:
    case OP_PLUSI:
    case OP_POSPLUS:
    case OP_POSPLUSI:
    case OP_POSQUERY:
    case OP_POSQUERYI:
    case OP_POSSTAR:
    case OP_POSSTARI:
    case OP_POSUPTO:
    case OP_POSUPTOI:
    case OP_QUERY:
    case OP_QUERYI:
    case OP_REF:
    case OP_REFI:
    case OP_SBRA:
    case OP_SBRAPOS:
    case OP_SCBRA:
    case OP_SCBRAPOS:
    case OP_SCOND:
    case OP_SKIPZERO:
    case OP_STAR:
    case OP_STARI:
    case OP_TYPEMINPLUS:
    case OP_TYPEMINQUERY:
    case OP_TYPEMINSTAR:
    case OP_TYPEMINUPTO:
    case OP_TYPEPLUS:
    case OP_TYPEPOSPLUS:
    case OP_TYPEPOSQUERY:
    case OP_TYPEPOSSTAR:
    case OP_TYPEPOSUPTO:
    case OP_TYPEQUERY:
    case OP_TYPESTAR:
    case OP_TYPEUPTO:
    case OP_UPTO:
    case OP_UPTOI:
    return -1;

    /* Catch unrecognized opcodes so that when new ones are added they
    are not forgotten, as has happened in the past. */

    default:
    return -4;
    }
  }
/* Control never gets here */
}




/*************************************************
*    Scan compiled regex for specific bracket    *
*************************************************/

/* This little function scans through a compiled pattern until it finds a
capturing bracket with the given number, or, if the number is negative, an
instance of OP_REVERSE for a lookbehind. The function is global in the C sense
so that it can be called from pcre_study() when finding the minimum matching
length.

Arguments:
  code        points to start of expression
  utf         TRUE in UTF-8 / UTF-16 mode
  number      the required bracket number or negative to find a lookbehind

Returns:      pointer to the opcode for the bracket, or NULL if not found
*/

const pcre_uchar *
PRIV(find_bracket)(const pcre_uchar *code, BOOL utf, int number)
{
for (;;)
  {
  register int c = *code;

  if (c == OP_END) return NULL;

  /* XCLASS is used for classes that cannot be represented just by a bit
  map. This includes negated single high-valued characters. The length in
  the table is zero; the actual length is stored in the compiled code. */

  if (c == OP_XCLASS) code += GET(code, 1);

  /* Handle recursion */

  else if (c == OP_REVERSE)
    {
    if (number < 0) return (pcre_uchar *)code;
    code += PRIV(OP_lengths)[c];
    }

  /* Handle capturing bracket */

  else if (c == OP_CBRA || c == OP_SCBRA ||
           c == OP_CBRAPOS || c == OP_SCBRAPOS)
    {
    int n = GET2(code, 1+LINK_SIZE);
    if (n == number) return (pcre_uchar *)code;
    code += PRIV(OP_lengths)[c];
    }

  /* Otherwise, we can get the item's length from the table, except that for
  repeated character types, we have to test for \p and \P, which have an extra
  two bytes of parameters, and for MARK/PRUNE/SKIP/THEN with an argument, we
  must add in its length. */

  else
    {
    switch(c)
      {
      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      case OP_TYPEPOSSTAR:
      case OP_TYPEPOSPLUS:
      case OP_TYPEPOSQUERY:
      if (code[1] == OP_PROP || code[1] == OP_NOTPROP) code += 2;
      break;

      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      case OP_TYPEEXACT:
      case OP_TYPEPOSUPTO:
      if (code[1 + IMM2_SIZE] == OP_PROP
        || code[1 + IMM2_SIZE] == OP_NOTPROP) code += 2;
      break;

      case OP_MARK:
      case OP_PRUNE_ARG:
      case OP_SKIP_ARG:
      code += code[1];
      break;

      case OP_THEN_ARG:
      code += code[1];
      break;
      }

    /* Add in the fixed length from the table */

    code += PRIV(OP_lengths)[c];

  /* In UTF-8 mode, opcodes that are followed by a character may be followed by
  a multi-byte character. The length in the table is a minimum, so we have to
  arrange to skip the extra bytes. */

#ifdef SUPPORT_UTF
    if (utf) switch(c)
      {
      case OP_CHAR:
      case OP_CHARI:
      case OP_EXACT:
      case OP_EXACTI:
      case OP_UPTO:
      case OP_UPTOI:
      case OP_MINUPTO:
      case OP_MINUPTOI:
      case OP_POSUPTO:
      case OP_POSUPTOI:
      case OP_STAR:
      case OP_STARI:
      case OP_MINSTAR:
      case OP_MINSTARI:
      case OP_POSSTAR:
      case OP_POSSTARI:
      case OP_PLUS:
      case OP_PLUSI:
      case OP_MINPLUS:
      case OP_MINPLUSI:
      case OP_POSPLUS:
      case OP_POSPLUSI:
      case OP_QUERY:
      case OP_QUERYI:
      case OP_MINQUERY:
      case OP_MINQUERYI:
      case OP_POSQUERY:
      case OP_POSQUERYI:
      if (HAS_EXTRALEN(code[-1])) code += GET_EXTRALEN(code[-1]);
      break;
      }
#else
    (void)(utf);  /* Keep compiler happy by referencing function argument */
#endif
    }
  }
}



/*************************************************
*   Scan compiled regex for recursion reference  *
*************************************************/

/* This little function scans through a compiled pattern until it finds an
instance of OP_RECURSE.

Arguments:
  code        points to start of expression
  utf         TRUE in UTF-8 / UTF-16 mode

Returns:      pointer to the opcode for OP_RECURSE, or NULL if not found
*/

static const pcre_uchar *
find_recurse(const pcre_uchar *code, BOOL utf)
{
for (;;)
  {
  register int c = *code;
  if (c == OP_END) return NULL;
  if (c == OP_RECURSE) return code;

  /* XCLASS is used for classes that cannot be represented just by a bit
  map. This includes negated single high-valued characters. The length in
  the table is zero; the actual length is stored in the compiled code. */

  if (c == OP_XCLASS) code += GET(code, 1);

  /* Otherwise, we can get the item's length from the table, except that for
  repeated character types, we have to test for \p and \P, which have an extra
  two bytes of parameters, and for MARK/PRUNE/SKIP/THEN with an argument, we
  must add in its length. */

  else
    {
    switch(c)
      {
      case OP_TYPESTAR:
      case OP_TYPEMINSTAR:
      case OP_TYPEPLUS:
      case OP_TYPEMINPLUS:
      case OP_TYPEQUERY:
      case OP_TYPEMINQUERY:
      case OP_TYPEPOSSTAR:
      case OP_TYPEPOSPLUS:
      case OP_TYPEPOSQUERY:
      if (code[1] == OP_PROP || code[1] == OP_NOTPROP) code += 2;
      break;

      case OP_TYPEPOSUPTO:
      case OP_TYPEUPTO:
      case OP_TYPEMINUPTO:
      case OP_TYPEEXACT:
      if (code[1 + IMM2_SIZE] == OP_PROP
        || code[1 + IMM2_SIZE] == OP_NOTPROP) code += 2;
      break;

      case OP_MARK:
      case OP_PRUNE_ARG:
      case OP_SKIP_ARG:
      code += code[1];
      break;

      case OP_THEN_ARG:
      code += code[1];
      break;
      }

    /* Add in the fixed length from the table */

    code += PRIV(OP_lengths)[c];

    /* In UTF-8 mode, opcodes that are followed by a character may be followed
    by a multi-byte character. The length in the table is a minimum, so we have
    to arrange to skip the extra bytes. */

#ifdef SUPPORT_UTF
    if (utf) switch(c)
      {
      case OP_CHAR:
      case OP_CHARI:
      case OP_EXACT:
      case OP_EXACTI:
      case OP_UPTO:
      case OP_UPTOI:
      case OP_MINUPTO:
      case OP_MINUPTOI:
      case OP_POSUPTO:
      case OP_POSUPTOI:
      case OP_STAR:
      case OP_STARI:
      case OP_MINSTAR:
      case OP_MINSTARI:
      case OP_POSSTAR:
      case OP_POSSTARI:
      case OP_PLUS:
      case OP_PLUSI:
      case OP_MINPLUS:
      case OP_MINPLUSI:
      case OP_POSPLUS:
      case OP_POSPLUSI:
      case OP_QUERY:
      case OP_QUERYI:
      case OP_MINQUERY:
      case OP_MINQUERYI:
      case OP_POSQUERY:
      case OP_POSQUERYI:
      if (HAS_EXTRALEN(code[-1])) code += GET_EXTRALEN(code[-1]);
      break;
      }
#else
    (void)(utf);  /* Keep compiler happy by referencing function argument */
#endif
    }
  }
}



/*************************************************
*    Scan compiled branch for non-emptiness      *
*************************************************/

/* This function scans through a branch of a compiled pattern to see whether it
can match the empty string or not. It is called from could_be_empty()
below and from compile_branch() when checking for an unlimited repeat of a
group that can match nothing. Note that first_significant_code() skips over
backward and negative forward assertions when its final argument is TRUE. If we
hit an unclosed bracket, we return "empty" - this means we've struck an inner
bracket whose current branch will already have been scanned.

Arguments:
  code        points to start of search
  endcode     points to where to stop
  utf         TRUE if in UTF-8 / UTF-16 mode
  cd          contains pointers to tables etc.

Returns:      TRUE if what is matched could be empty
*/

static BOOL
could_be_empty_branch(const pcre_uchar *code, const pcre_uchar *endcode,
  BOOL utf, compile_data *cd)
{
register int c;
for (code = first_significant_code(code + PRIV(OP_lengths)[*code], TRUE);
     code < endcode;
     code = first_significant_code(code + PRIV(OP_lengths)[c], TRUE))
  {
  const pcre_uchar *ccode;

  c = *code;

  /* Skip over forward assertions; the other assertions are skipped by
  first_significant_code() with a TRUE final argument. */

  if (c == OP_ASSERT)
    {
    do code += GET(code, 1); while (*code == OP_ALT);
    c = *code;
    continue;
    }

  /* For a recursion/subroutine call, if its end has been reached, which
  implies a backward reference subroutine call, we can scan it. If it's a
  forward reference subroutine call, we can't. To detect forward reference
  we have to scan up the list that is kept in the workspace. This function is
  called only when doing the real compile, not during the pre-compile that
  measures the size of the compiled pattern. */

  if (c == OP_RECURSE)
    {
    const pcre_uchar *scode;
    BOOL empty_branch;

    /* Test for forward reference */

    for (scode = cd->start_workspace; scode < cd->hwm; scode += LINK_SIZE)
      if (GET(scode, 0) == code + 1 - cd->start_code) return TRUE;

    /* Not a forward reference, test for completed backward reference */

    empty_branch = FALSE;
    scode = cd->start_code + GET(code, 1);
    if (GET(scode, 1) == 0) return TRUE;    /* Unclosed */

    /* Completed backwards reference */

    do
      {
      if (could_be_empty_branch(scode, endcode, utf, cd))
        {
        empty_branch = TRUE;
        break;
        }
      scode += GET(scode, 1);
      }
    while (*scode == OP_ALT);

    if (!empty_branch) return FALSE;  /* All branches are non-empty */
    continue;
    }

  /* Groups with zero repeats can of course be empty; skip them. */

  if (c == OP_BRAZERO || c == OP_BRAMINZERO || c == OP_SKIPZERO ||
      c == OP_BRAPOSZERO)
    {
    code += PRIV(OP_lengths)[c];
    do code += GET(code, 1); while (*code == OP_ALT);
    c = *code;
    continue;
    }

  /* A nested group that is already marked as "could be empty" can just be
  skipped. */

  if (c == OP_SBRA  || c == OP_SBRAPOS ||
      c == OP_SCBRA || c == OP_SCBRAPOS)
    {
    do code += GET(code, 1); while (*code == OP_ALT);
    c = *code;
    continue;
    }

  /* For other groups, scan the branches. */

  if (c == OP_BRA  || c == OP_BRAPOS ||
      c == OP_CBRA || c == OP_CBRAPOS ||
      c == OP_ONCE || c == OP_ONCE_NC ||
      c == OP_COND)
    {
    BOOL empty_branch;
    if (GET(code, 1) == 0) return TRUE;    /* Hit unclosed bracket */

    /* If a conditional group has only one branch, there is a second, implied,
    empty branch, so just skip over the conditional, because it could be empty.
    Otherwise, scan the individual branches of the group. */

    if (c == OP_COND && code[GET(code, 1)] != OP_ALT)
      code += GET(code, 1);
    else
      {
      empty_branch = FALSE;
      do
        {
        if (!empty_branch && could_be_empty_branch(code, endcode, utf, cd))
          empty_branch = TRUE;
        code += GET(code, 1);
        }
      while (*code == OP_ALT);
      if (!empty_branch) return FALSE;   /* All branches are non-empty */
      }

    c = *code;
    continue;
    }

  /* Handle the other opcodes */

  switch (c)
    {
    /* Check for quantifiers after a class. XCLASS is used for classes that
    cannot be represented just by a bit map. This includes negated single
    high-valued characters. The length in PRIV(OP_lengths)[] is zero; the
    actual length is stored in the compiled code, so we must update "code"
    here. */

#if defined SUPPORT_UTF || !defined COMPILE_PCRE8
    case OP_XCLASS:
    ccode = code += GET(code, 1);
    goto CHECK_CLASS_REPEAT;
#endif

    case OP_CLASS:
    case OP_NCLASS:
    ccode = code + PRIV(OP_lengths)[OP_CLASS];

#if defined SUPPORT_UTF || !defined COMPILE_PCRE8
    CHECK_CLASS_REPEAT:
#endif

    switch (*ccode)
      {
      case OP_CRSTAR:            /* These could be empty; continue */
      case OP_CRMINSTAR:
      case OP_CRQUERY:
      case OP_CRMINQUERY:
      break;

      default:                   /* Non-repeat => class must match */
      case OP_CRPLUS:            /* These repeats aren't empty */
      case OP_CRMINPLUS:
      return FALSE;

      case OP_CRRANGE:
      case OP_CRMINRANGE:
      if (GET2(ccode, 1) > 0) return FALSE;  /* Minimum > 0 */
      break;
      }
    break;

    /* Opcodes that must match a character */

    case OP_PROP:
    case OP_NOTPROP:
    case OP_EXTUNI:
    case OP_NOT_DIGIT:
    case OP_DIGIT:
    case OP_NOT_WHITESPACE:
    case OP_WHITESPACE:
    case OP_NOT_WORDCHAR:
    case OP_WORDCHAR:
    case OP_ANY:
    case OP_ALLANY:
    case OP_ANYBYTE:
    case OP_CHAR:
    case OP_CHARI:
    case OP_NOT:
    case OP_NOTI:
    case OP_PLUS:
    case OP_MINPLUS:
    case OP_POSPLUS:
    case OP_EXACT:
    case OP_NOTPLUS:
    case OP_NOTMINPLUS:
    case OP_NOTPOSPLUS:
    case OP_NOTEXACT:
    case OP_TYPEPLUS:
    case OP_TYPEMINPLUS:
    case OP_TYPEPOSPLUS:
    case OP_TYPEEXACT:
    return FALSE;

    /* These are going to continue, as they may be empty, but we have to
    fudge the length for the \p and \P cases. */

    case OP_TYPESTAR:
    case OP_TYPEMINSTAR:
    case OP_TYPEPOSSTAR:
    case OP_TYPEQUERY:
    case OP_TYPEMINQUERY:
    case OP_TYPEPOSQUERY:
    if (code[1] == OP_PROP || code[1] == OP_NOTPROP) code += 2;
    break;

    /* Same for these */

    case OP_TYPEUPTO:
    case OP_TYPEMINUPTO:
    case OP_TYPEPOSUPTO:
    if (code[1 + IMM2_SIZE] == OP_PROP
      || code[1 + IMM2_SIZE] == OP_NOTPROP) code += 2;
    break;

    /* End of branch */

    case OP_KET:
    case OP_KETRMAX:
    case OP_KETRMIN:
    case OP_KETRPOS:
    case OP_ALT:
    return TRUE;

    /* In UTF-8 mode, STAR, MINSTAR, POSSTAR, QUERY, MINQUERY, POSQUERY, UPTO,
    MINUPTO, and POSUPTO may be followed by a multibyte character */

#ifdef SUPPORT_UTF
    case OP_STAR:
    case OP_STARI:
    case OP_MINSTAR:
    case OP_MINSTARI:
    case OP_POSSTAR:
    case OP_POSSTARI:
    case OP_QUERY:
    case OP_QUERYI:
    case OP_MINQUERY:
    case OP_MINQUERYI:
    case OP_POSQUERY:
    case OP_POSQUERYI:
    if (utf && HAS_EXTRALEN(code[1])) code += GET_EXTRALEN(code[1]);
    break;

    case OP_UPTO:
    case OP_UPTOI:
    case OP_MINUPTO:
    case OP_MINUPTOI:
    case OP_POSUPTO:
    case OP_POSUPTOI:
    if (utf && HAS_EXTRALEN(code[1 + IMM2_SIZE])) code += GET_EXTRALEN(code[1 + IMM2_SIZE]);
    break;
#endif

    /* MARK, and PRUNE/SKIP/THEN with an argument must skip over the argument
    string. */

    case OP_MARK:
    case OP_PRUNE_ARG:
    case OP_SKIP_ARG:
    code += code[1];
    break;

    case OP_THEN_ARG:
    code += code[1];
    break;

    /* None of the remaining opcodes are required to match a character. */

    default:
    break;
    }
  }

return TRUE;
}



/*************************************************
*    Scan compiled regex for non-emptiness       *
*************************************************/

/* This function is called to check for left recursive calls. We want to check
the current branch of the current pattern to see if it could match the empty
string. If it could, we must look outwards for branches at other levels,
stopping when we pass beyond the bracket which is the subject of the recursion.
This function is called only during the real compile, not during the
pre-compile.

Arguments:
  code        points to start of the recursion
  endcode     points to where to stop (current RECURSE item)
  bcptr       points to the chain of current (unclosed) branch starts
  utf         TRUE if in UTF-8 / UTF-16 mode
  cd          pointers to tables etc

Returns:      TRUE if what is matched could be empty
*/

static BOOL
could_be_empty(const pcre_uchar *code, const pcre_uchar *endcode,
  branch_chain *bcptr, BOOL utf, compile_data *cd)
{
while (bcptr != NULL && bcptr->current_branch >= code)
  {
  if (!could_be_empty_branch(bcptr->current_branch, endcode, utf, cd))
    return FALSE;
  bcptr = bcptr->outer;
  }
return TRUE;
}



/*************************************************
*           Check for POSIX class syntax         *
*************************************************/

/* This function is called when the sequence "[:" or "[." or "[=" is
encountered in a character class. It checks whether this is followed by a
sequence of characters terminated by a matching ":]" or ".]" or "=]". If we
reach an unescaped ']' without the special preceding character, return FALSE.

Originally, this function only recognized a sequence of letters between the
terminators, but it seems that Perl recognizes any sequence of characters,
though of course unknown POSIX names are subsequently rejected. Perl gives an
"Unknown POSIX class" error for [:f\oo:] for example, where previously PCRE
didn't consider this to be a POSIX class. Likewise for [:1234:].

The problem in trying to be exactly like Perl is in the handling of escapes. We
have to be sure that [abc[:x\]pqr] is *not* treated as containing a POSIX
class, but [abc[:x\]pqr:]] is (so that an error can be generated). The code
below handles the special case of \], but does not try to do any other escape
processing. This makes it different from Perl for cases such as [:l\ower:]
where Perl recognizes it as the POSIX class "lower" but PCRE does not recognize
"l\ower". This is a lesser evil that not diagnosing bad classes when Perl does,
I think.

A user pointed out that PCRE was rejecting [:a[:digit:]] whereas Perl was not.
It seems that the appearance of a nested POSIX class supersedes an apparent
external class. For example, [:a[:digit:]b:] matches "a", "b", ":", or
a digit.

In Perl, unescaped square brackets may also appear as part of class names. For
example, [:a[:abc]b:] gives unknown POSIX class "[:abc]b:]". However, for
[:a[:abc]b][b:] it gives unknown POSIX class "[:abc]b][b:]", which does not
seem right at all. PCRE does not allow closing square brackets in POSIX class
names.

Arguments:
  ptr      pointer to the initial [
  endptr   where to return the end pointer

Returns:   TRUE or FALSE
*/

static BOOL
check_posix_syntax(const pcre_uchar *ptr, const pcre_uchar **endptr)
{
int terminator;          /* Don't combine these lines; the Solaris cc */
terminator = *(++ptr);   /* compiler warns about "non-constant" initializer. */
for (++ptr; *ptr != 0; ptr++)
  {
  if (*ptr == CHAR_BACKSLASH && ptr[1] == CHAR_RIGHT_SQUARE_BRACKET)
    ptr++;
  else if (*ptr == CHAR_RIGHT_SQUARE_BRACKET) return FALSE;
  else
    {
    if (*ptr == terminator && ptr[1] == CHAR_RIGHT_SQUARE_BRACKET)
      {
      *endptr = ptr;
      return TRUE;
      }
    if (*ptr == CHAR_LEFT_SQUARE_BRACKET &&
         (ptr[1] == CHAR_COLON || ptr[1] == CHAR_DOT ||
          ptr[1] == CHAR_EQUALS_SIGN) &&
        check_posix_syntax(ptr, endptr))
      return FALSE;
    }
  }
return FALSE;
}




/*************************************************
*          Check POSIX class name                *
*************************************************/

/* This function is called to check the name given in a POSIX-style class entry
such as [:alnum:].

Arguments:
  ptr        points to the first letter
  len        the length of the name

Returns:     a value representing the name, or -1 if unknown
*/

static int
check_posix_name(const pcre_uchar *ptr, int len)
{
const char *pn = posix_names;
register int yield = 0;
while (posix_name_lengths[yield] != 0)
  {
  if (len == posix_name_lengths[yield] &&
    STRNCMP_UC_C8(ptr, pn, len) == 0) return yield;
  pn += posix_name_lengths[yield] + 1;
  yield++;
  }
return -1;
}


/*************************************************
*    Adjust OP_RECURSE items in repeated group   *
*************************************************/

/* OP_RECURSE items contain an offset from the start of the regex to the group
that is referenced. This means that groups can be replicated for fixed
repetition simply by copying (because the recursion is allowed to refer to
earlier groups that are outside the current group). However, when a group is
optional (i.e. the minimum quantifier is zero), OP_BRAZERO or OP_SKIPZERO is
inserted before it, after it has been compiled. This means that any OP_RECURSE
items within it that refer to the group itself or any contained groups have to
have their offsets adjusted. That one of the jobs of this function. Before it
is called, the partially compiled regex must be temporarily terminated with
OP_END.

This function has been extended with the possibility of forward references for
recursions and subroutine calls. It must also check the list of such references
for the group we are dealing with. If it finds that one of the recursions in
the current group is on this list, it adjusts the offset in the list, not the
value in the reference (which is a group number).

Arguments:
  group      points to the start of the group
  adjust     the amount by which the group is to be moved
  utf        TRUE in UTF-8 / UTF-16 mode
  cd         contains pointers to tables etc.
  save_hwm   the hwm forward reference pointer at the start of the group

Returns:     nothing
*/

static void
adjust_recurse(pcre_uchar *group, int adjust, BOOL utf, compile_data *cd,
  pcre_uchar *save_hwm)
{
pcre_uchar *ptr = group;

while ((ptr = (pcre_uchar *)find_recurse(ptr, utf)) != NULL)
  {
  int offset;
  pcre_uchar *hc;

  /* See if this recursion is on the forward reference list. If so, adjust the
  reference. */

  for (hc = save_hwm; hc < cd->hwm; hc += LINK_SIZE)
    {
    offset = GET(hc, 0);
    if (cd->start_code + offset == ptr + 1)
      {
      PUT(hc, 0, offset + adjust);
      break;
      }
    }

  /* Otherwise, adjust the recursion offset if it's after the start of this
  group. */

  if (hc >= cd->hwm)
    {
    offset = GET(ptr, 1);
    if (cd->start_code + offset >= group) PUT(ptr, 1, offset + adjust);
    }

  ptr += 1 + LINK_SIZE;
  }
}



/*************************************************
*        Insert an automatic callout point       *
*************************************************/

/* This function is called when the PCRE_AUTO_CALLOUT option is set, to insert
callout points before each pattern item.

Arguments:
  code           current code pointer
  ptr            current pattern pointer
  cd             pointers to tables etc

Returns:         new code pointer
*/

static pcre_uchar *
auto_callout(pcre_uchar *code, const pcre_uchar *ptr, compile_data *cd)
{
// *code++ = OP_CALLOUT;
// *code++ = 255;
// *((void **)code)++ = NULL;
// PUT(code, 0, (int)(ptr - cd->start_pattern));  /* Pattern offset */
// PUT(code, LINK_SIZE, 0);                       /* Default length */
// return code + 2 * LINK_SIZE;
*code++ = OP_CALLOUT;
*code++ = 0; /* AutoHotkey: zero length name */
*code++ = 0; /* AutoHotkey: null terminator */
*code++ = 255;
PUT(code, 0, (int)(ptr - cd->start_pattern));  /* Pattern offset */
PUT(code, LINK_SIZE, 0);                       /* Default length */
return code + 2 * LINK_SIZE;
}



/*************************************************
*         Complete a callout item                *
*************************************************/

/* A callout item contains the length of the next item in the pattern, which
we can't fill in till after we have reached the relevant point. This is used
for both automatic and manual callouts.

Arguments:
  previous_callout   points to previous callout item
  ptr                current pattern pointer
  cd                 pointers to tables etc

Returns:             nothing
*/

static void
complete_callout(pcre_uchar *previous_callout, const pcre_uchar *ptr, compile_data *cd)
{
int length = (int)(ptr - cd->start_pattern - GET(previous_callout, 2 + IMMPTR_SIZE));
PUT(previous_callout, 2 + IMMPTR_SIZE + LINK_SIZE, length);
}



#ifdef SUPPORT_UCP
/*************************************************
*           Get othercase range                  *
*************************************************/

/* This function is passed the start and end of a class range, in UTF-8 mode
with UCP support. It searches up the characters, looking for internal ranges of
characters in the "other" case. Each call returns the next one, updating the
start address.

Arguments:
  cptr        points to starting character value; updated
  d           end value
  ocptr       where to put start of othercase range
  odptr       where to put end of othercase range

Yield:        TRUE when range returned; FALSE when no more
*/

static BOOL
get_othercase_range(unsigned int *cptr, unsigned int d, unsigned int *ocptr,
  unsigned int *odptr)
{
unsigned int c, othercase, next;

for (c = *cptr; c <= d; c++)
  { if ((othercase = UCD_OTHERCASE(c)) != c) break; }

if (c > d) return FALSE;

*ocptr = othercase;
next = othercase + 1;

for (++c; c <= d; c++)
  {
  if (UCD_OTHERCASE(c) != next) break;
  next++;
  }

*odptr = next - 1;
*cptr = c;

return TRUE;
}



/*************************************************
*        Check a character and a property        *
*************************************************/

/* This function is called by check_auto_possessive() when a property item
is adjacent to a fixed character.

Arguments:
  c            the character
  ptype        the property type
  pdata        the data for the type
  negated      TRUE if it's a negated property (\P or \p{^)

Returns:       TRUE if auto-possessifying is OK
*/

static BOOL
check_char_prop(int c, int ptype, int pdata, BOOL negated)
{
const ucd_record *prop = GET_UCD(c);
switch(ptype)
  {
  case PT_LAMP:
  return (prop->chartype == ucp_Lu ||
          prop->chartype == ucp_Ll ||
          prop->chartype == ucp_Lt) == negated;

  case PT_GC:
  return (pdata == PRIV(ucp_gentype)[prop->chartype]) == negated;

  case PT_PC:
  return (pdata == prop->chartype) == negated;

  case PT_SC:
  return (pdata == prop->script) == negated;

  /* These are specials */

  case PT_ALNUM:
  return (PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
          PRIV(ucp_gentype)[prop->chartype] == ucp_N) == negated;

  case PT_SPACE:    /* Perl space */
  return (PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
          c == CHAR_HT || c == CHAR_NL || c == CHAR_FF || c == CHAR_CR)
          == negated;

  case PT_PXSPACE:  /* POSIX space */
  return (PRIV(ucp_gentype)[prop->chartype] == ucp_Z ||
          c == CHAR_HT || c == CHAR_NL || c == CHAR_VT ||
          c == CHAR_FF || c == CHAR_CR)
          == negated;

  case PT_WORD:
  return (PRIV(ucp_gentype)[prop->chartype] == ucp_L ||
          PRIV(ucp_gentype)[prop->chartype] == ucp_N ||
          c == CHAR_UNDERSCORE) == negated;
  }
return FALSE;
}
#endif  /* SUPPORT_UCP */



/*************************************************
*     Check if auto-possessifying is possible    *
*************************************************/

/* This function is called for unlimited repeats of certain items, to see
whether the next thing could possibly match the repeated item. If not, it makes
sense to automatically possessify the repeated item.

Arguments:
  previous      pointer to the repeated opcode
  utf           TRUE in UTF-8 / UTF-16 mode
  ptr           next character in pattern
  options       options bits
  cd            contains pointers to tables etc.

Returns:        TRUE if possessifying is wanted
*/

static BOOL
check_auto_possessive(const pcre_uchar *previous, BOOL utf,
  const pcre_uchar *ptr, int options, compile_data *cd)
{
pcre_int32 c, next;
int op_code = *previous++;

/* Skip whitespace and comments in extended mode */

if ((options & PCRE_EXTENDED) != 0)
  {
  for (;;)
    {
    while (MAX_255(*ptr) && (cd->ctypes[*ptr] & ctype_space) != 0) ptr++;
    if (*ptr == CHAR_NUMBER_SIGN)
      {
      ptr++;
      while (*ptr != 0)
        {
        if (IS_NEWLINE(ptr)) { ptr += cd->nllen; break; }
        ptr++;
#ifdef SUPPORT_UTF
        if (utf) FORWARDCHAR(ptr);
#endif
        }
      }
    else break;
    }
  }

/* If the next item is one that we can handle, get its value. A non-negative
value is a character, a negative value is an escape value. */

if (*ptr == CHAR_BACKSLASH)
  {
  int temperrorcode = 0;
  next = check_escape(&ptr, &temperrorcode, cd->bracount, options, FALSE);
  if (temperrorcode != 0) return FALSE;
  ptr++;    /* Point after the escape sequence */
  }
else if (!MAX_255(*ptr) || (cd->ctypes[*ptr] & ctype_meta) == 0)
  {
#ifdef SUPPORT_UTF
  if (utf) { GETCHARINC(next, ptr); } else
#endif
  next = *ptr++;
  }
else return FALSE;

/* Skip whitespace and comments in extended mode */

if ((options & PCRE_EXTENDED) != 0)
  {
  for (;;)
    {
    while (MAX_255(*ptr) && (cd->ctypes[*ptr] & ctype_space) != 0) ptr++;
    if (*ptr == CHAR_NUMBER_SIGN)
      {
      ptr++;
      while (*ptr != 0)
        {
        if (IS_NEWLINE(ptr)) { ptr += cd->nllen; break; }
        ptr++;
#ifdef SUPPORT_UTF
        if (utf) FORWARDCHAR(ptr);
#endif
        }
      }
    else break;
    }
  }

/* If the next thing is itself optional, we have to give up. */

if (*ptr == CHAR_ASTERISK || *ptr == CHAR_QUESTION_MARK ||
  STRNCMP_UC_C8(ptr, STR_LEFT_CURLY_BRACKET STR_0 STR_COMMA, 3) == 0)
    return FALSE;

/* Now compare the next item with the previous opcode. First, handle cases when
the next item is a character. */

if (next >= 0) switch(op_code)
  {
  case OP_CHAR:
#ifdef SUPPORT_UTF
  GETCHARTEST(c, previous);
#else
  c = *previous;
#endif
  return c != next;

  /* For CHARI (caseless character) we must check the other case. If we have
  Unicode property support, we can use it to test the other case of
  high-valued characters. */

  case OP_CHARI:
#ifdef SUPPORT_UTF
  GETCHARTEST(c, previous);
#else
  c = *previous;
#endif
  if (c == next) return FALSE;
#ifdef SUPPORT_UTF
  if (utf)
    {
    unsigned int othercase;
    if (next < 128) othercase = cd->fcc[next]; else
#ifdef SUPPORT_UCP
    othercase = UCD_OTHERCASE((unsigned int)next);
#else
    othercase = NOTACHAR;
#endif
    return (unsigned int)c != othercase;
    }
  else
#endif  /* SUPPORT_UTF */
  return (c != TABLE_GET((unsigned int)next, cd->fcc, next));  /* Non-UTF-8 mode */

  /* For OP_NOT and OP_NOTI, the data is always a single-byte character. These
  opcodes are not used for multi-byte characters, because they are coded using
  an XCLASS instead. */

  case OP_NOT:
  return (c = *previous) == next;

  case OP_NOTI:
  if ((c = *previous) == next) return TRUE;
#ifdef SUPPORT_UTF
  if (utf)
    {
    unsigned int othercase;
    if (next < 128) othercase = cd->fcc[next]; else
#ifdef SUPPORT_UCP
    othercase = UCD_OTHERCASE(next);
#else
    othercase = NOTACHAR;
#endif
    return (unsigned int)c == othercase;
    }
  else
#endif  /* SUPPORT_UTF */
  return (c == (int)(TABLE_GET((unsigned int)next, cd->fcc, next)));  /* Non-UTF-8 mode */

  /* Note that OP_DIGIT etc. are generated only when PCRE_UCP is *not* set.
  When it is set, \d etc. are converted into OP_(NOT_)PROP codes. */

  case OP_DIGIT:
  return next > 127 || (cd->ctypes[next] & ctype_digit) == 0;

  case OP_NOT_DIGIT:
  return next <= 127 && (cd->ctypes[next] & ctype_digit) != 0;

  case OP_WHITESPACE:
  return next > 127 || (cd->ctypes[next] & ctype_space) == 0;

  case OP_NOT_WHITESPACE:
  return next <= 127 && (cd->ctypes[next] & ctype_space) != 0;

  case OP_WORDCHAR:
  return next > 127 || (cd->ctypes[next] & ctype_word) == 0;

  case OP_NOT_WORDCHAR:
  return next <= 127 && (cd->ctypes[next] & ctype_word) != 0;

  case OP_HSPACE:
  case OP_NOT_HSPACE:
  switch(next)
    {
    case 0x09:
    case 0x20:
    case 0xa0:
    case 0x1680:
    case 0x180e:
    case 0x2000:
    case 0x2001:
    case 0x2002:
    case 0x2003:
    case 0x2004:
    case 0x2005:
    case 0x2006:
    case 0x2007:
    case 0x2008:
    case 0x2009:
    case 0x200A:
    case 0x202f:
    case 0x205f:
    case 0x3000:
    return op_code == OP_NOT_HSPACE;
    default:
    return op_code != OP_NOT_HSPACE;
    }

  case OP_ANYNL:
  case OP_VSPACE:
  case OP_NOT_VSPACE:
  switch(next)
    {
    case 0x0a:
    case 0x0b:
    case 0x0c:
    case 0x0d:
    case 0x85:
    case 0x2028:
    case 0x2029:
    return op_code == OP_NOT_VSPACE;
    default:
    return op_code != OP_NOT_VSPACE;
    }

#ifdef SUPPORT_UCP
  case OP_PROP:
  return check_char_prop(next, previous[0], previous[1], FALSE);

  case OP_NOTPROP:
  return check_char_prop(next, previous[0], previous[1], TRUE);
#endif

  default:
  return FALSE;
  }


/* Handle the case when the next item is \d, \s, etc. Note that when PCRE_UCP
is set, \d turns into ESC_du rather than ESC_d, etc., so ESC_d etc. are
generated only when PCRE_UCP is *not* set, that is, when only ASCII
characteristics are recognized. Similarly, the opcodes OP_DIGIT etc. are
replaced by OP_PROP codes when PCRE_UCP is set. */

switch(op_code)
  {
  case OP_CHAR:
  case OP_CHARI:
#ifdef SUPPORT_UTF
  GETCHARTEST(c, previous);
#else
  c = *previous;
#endif
  switch(-next)
    {
    case ESC_d:
    return c > 127 || (cd->ctypes[c] & ctype_digit) == 0;

    case ESC_D:
    return c <= 127 && (cd->ctypes[c] & ctype_digit) != 0;

    case ESC_s:
    return c > 127 || (cd->ctypes[c] & ctype_space) == 0;

    case ESC_S:
    return c <= 127 && (cd->ctypes[c] & ctype_space) != 0;

    case ESC_w:
    return c > 127 || (cd->ctypes[c] & ctype_word) == 0;

    case ESC_W:
    return c <= 127 && (cd->ctypes[c] & ctype_word) != 0;

    case ESC_h:
    case ESC_H:
    switch(c)
      {
      case 0x09:
      case 0x20:
      case 0xa0:
      case 0x1680:
      case 0x180e:
      case 0x2000:
      case 0x2001:
      case 0x2002:
      case 0x2003:
      case 0x2004:
      case 0x2005:
      case 0x2006:
      case 0x2007:
      case 0x2008:
      case 0x2009:
      case 0x200A:
      case 0x202f:
      case 0x205f:
      case 0x3000:
      return -next != ESC_h;
      default:
      return -next == ESC_h;
      }

    case ESC_v:
    case ESC_V:
    switch(c)
      {
      case 0x0a:
      case 0x0b:
      case 0x0c:
      case 0x0d:
      case 0x85:
      case 0x2028:
      case 0x2029:
      return -next != ESC_v;
      default:
      return -next == ESC_v;
      }

    /* When PCRE_UCP is set, these values get generated for \d etc. Find
    their substitutions and process them. The result will always be either
    -ESC_p or -ESC_P. Then fall through to process those values. */

#ifdef SUPPORT_UCP
    case ESC_du:
    case ESC_DU:
    case ESC_wu:
    case ESC_WU:
    case ESC_su:
    case ESC_SU:
      {
      int temperrorcode = 0;
      ptr = substitutes[-next - ESC_DU];
      next = check_escape(&ptr, &temperrorcode, 0, options, FALSE);
      if (temperrorcode != 0) return FALSE;
      ptr++;    /* For compatibility */
      }
    /* Fall through */

    case ESC_p:
    case ESC_P:
      {
      int ptype, pdata, errorcodeptr;
      BOOL negated;

      ptr--;      /* Make ptr point at the p or P */
      ptype = get_ucp(&ptr, &negated, &pdata, &errorcodeptr);
      if (ptype < 0) return FALSE;
      ptr++;      /* Point past the final curly ket */

      /* If the property item is optional, we have to give up. (When generated
      from \d etc by PCRE_UCP, this test will have been applied much earlier,
      to the original \d etc. At this point, ptr will point to a zero byte. */

      if (*ptr == CHAR_ASTERISK || *ptr == CHAR_QUESTION_MARK ||
        STRNCMP_UC_C8(ptr, STR_LEFT_CURLY_BRACKET STR_0 STR_COMMA, 3) == 0)
          return FALSE;

      /* Do the property check. */

      return check_char_prop(c, ptype, pdata, (next == -ESC_P) != negated);
      }
#endif

    default:
    return FALSE;
    }

  /* In principle, support for Unicode properties should be integrated here as
  well. It means re-organizing the above code so as to get hold of the property
  values before switching on the op-code. However, I wonder how many patterns
  combine ASCII \d etc with Unicode properties? (Note that if PCRE_UCP is set,
  these op-codes are never generated.) */

  case OP_DIGIT:
  return next == -ESC_D || next == -ESC_s || next == -ESC_W ||
         next == -ESC_h || next == -ESC_v || next == -ESC_R;

  case OP_NOT_DIGIT:
  return next == -ESC_d;

  case OP_WHITESPACE:
  return next == -ESC_S || next == -ESC_d || next == -ESC_w || next == -ESC_R;

  case OP_NOT_WHITESPACE:
  return next == -ESC_s || next == -ESC_h || next == -ESC_v;

  case OP_HSPACE:
  return next == -ESC_S || next == -ESC_H || next == -ESC_d ||
         next == -ESC_w || next == -ESC_v || next == -ESC_R;

  case OP_NOT_HSPACE:
  return next == -ESC_h;

  /* Can't have \S in here because VT matches \S (Perl anomaly) */
  case OP_ANYNL:
  case OP_VSPACE:
  return next == -ESC_V || next == -ESC_d || next == -ESC_w;

  case OP_NOT_VSPACE:
  return next == -ESC_v || next == -ESC_R;

  case OP_WORDCHAR:
  return next == -ESC_W || next == -ESC_s || next == -ESC_h ||
         next == -ESC_v || next == -ESC_R;

  case OP_NOT_WORDCHAR:
  return next == -ESC_w || next == -ESC_d;

  default:
  return FALSE;
  }

/* Control does not reach here */
}



/*************************************************
*           Compile one branch                   *
*************************************************/

/* Scan the pattern, compiling it into the a vector. If the options are
changed during the branch, the pointer is used to change the external options
bits. This function is used during the pre-compile phase when we are trying
to find out the amount of memory needed, as well as during the real compile
phase. The value of lengthptr distinguishes the two phases.

Arguments:
  optionsptr     pointer to the option bits
  codeptr        points to the pointer to the current code point
  ptrptr         points to the current pattern pointer
  errorcodeptr   points to error code variable
  firstcharptr   set to initial literal character, or < 0 (REQ_UNSET, REQ_NONE)
  reqcharptr     set to the last literal character required, else < 0
  bcptr          points to current branch chain
  cond_depth     conditional nesting depth
  cd             contains pointers to tables etc.
  lengthptr      NULL during the real compile phase
                 points to length accumulator during pre-compile phase

Returns:         TRUE on success
                 FALSE, with *errorcodeptr set non-zero on error
*/

static BOOL
compile_branch(int *optionsptr, pcre_uchar **codeptr,
  const pcre_uchar **ptrptr, int *errorcodeptr, pcre_int32 *firstcharptr,
  pcre_int32 *reqcharptr, branch_chain *bcptr, int cond_depth,
  compile_data *cd, int *lengthptr)
{
int repeat_type, op_type;
int repeat_min = 0, repeat_max = 0;      /* To please picky compilers */
int bravalue = 0;
int greedy_default, greedy_non_default;
pcre_int32 firstchar, reqchar;
pcre_int32 zeroreqchar, zerofirstchar;
pcre_int32 req_caseopt, reqvary, tempreqvary;
int options = *optionsptr;               /* May change dynamically */
int after_manual_callout = 0;
int length_prevgroup = 0;
register int c;
register pcre_uchar *code = *codeptr;
pcre_uchar *last_code = code;
pcre_uchar *orig_code = code;
pcre_uchar *tempcode;
BOOL inescq = FALSE;
BOOL groupsetfirstchar = FALSE;
const pcre_uchar *ptr = *ptrptr;
const pcre_uchar *tempptr;
const pcre_uchar *nestptr = NULL;
pcre_uchar *previous = NULL;
pcre_uchar *previous_callout = NULL;
pcre_uchar *save_hwm = NULL;
pcre_uint8 classbits[32];

/* We can fish out the UTF-8 setting once and for all into a BOOL, but we
must not do this for other options (e.g. PCRE_EXTENDED) because they may change
dynamically as we process the pattern. */

#ifdef SUPPORT_UTF
/* PCRE_UTF16 has the same value as PCRE_UTF8. */
BOOL utf = UTF_ENABLED((options & PCRE_UTF8) != 0);
pcre_uchar utf_chars[6];
#else
BOOL utf = FALSE;
#endif

/* Helper variables for OP_XCLASS opcode (for characters > 255). */

#if defined SUPPORT_UTF || !defined COMPILE_PCRE8
BOOL xclass;
pcre_uchar *class_uchardata;
pcre_uchar *class_uchardata_base;
#endif

#ifdef PCRE_DEBUG
if (lengthptr != NULL) DPRINTF((">> start branch\n"));
#endif

/* Set up the default and non-default settings for greediness */

greedy_default = ((options & PCRE_UNGREEDY) != 0);
greedy_non_default = greedy_default ^ 1;

/* Initialize no first byte, no required byte. REQ_UNSET means "no char
matching encountered yet". It gets changed to REQ_NONE if we hit something that
matches a non-fixed char first char; reqchar just remains unset if we never
find one.

When we hit a repeat whose minimum is zero, we may have to adjust these values
to take the zero repeat into account. This is implemented by setting them to
zerofirstbyte and zeroreqchar when such a repeat is encountered. The individual
item types that can be repeated set these backoff variables appropriately. */

firstchar = reqchar = zerofirstchar = zeroreqchar = REQ_UNSET;

/* The variable req_caseopt contains either the REQ_CASELESS value
or zero, according to the current setting of the caseless flag. The
REQ_CASELESS leaves the lower 28 bit empty. It is added into the
firstchar or reqchar variables to record the case status of the
value. This is used only for ASCII characters. */

req_caseopt = ((options & PCRE_CASELESS) != 0)? REQ_CASELESS:0;

/* Switch on next character until the end of the branch */

for (;; ptr++)
  {
  BOOL negate_class;
  BOOL should_flip_negation;
  BOOL possessive_quantifier;
  BOOL is_quantifier;
  BOOL is_recurse;
  BOOL reset_bracount;
  int class_has_8bitchar;
  int class_single_char;
  int newoptions;
  int recno;
  int refsign;
  int skipbytes;
  int subreqchar;
  int subfirstchar;
  int terminator;
  int mclength;
  int tempbracount;
  pcre_uchar mcbuffer[8];

  /* Get next character in the pattern */

  c = *ptr;

  /* If we are at the end of a nested substitution, revert to the outer level
  string. Nesting only happens one level deep. */

  if (c == 0 && nestptr != NULL)
    {
    ptr = nestptr;
    nestptr = NULL;
    c = *ptr;
    }

  /* If we are in the pre-compile phase, accumulate the length used for the
  previous cycle of this loop. */

  if (lengthptr != NULL)
    {
#ifdef PCRE_DEBUG
    if (code > cd->hwm) cd->hwm = code;                 /* High water info */
#endif
    if (code > cd->start_workspace + cd->workspace_size -
        WORK_SIZE_SAFETY_MARGIN)                       /* Check for overrun */
      {
      *errorcodeptr = ERR52;
      goto FAILED;
      }

    /* There is at least one situation where code goes backwards: this is the
    case of a zero quantifier after a class (e.g. [ab]{0}). At compile time,
    the class is simply eliminated. However, it is created first, so we have to
    allow memory for it. Therefore, don't ever reduce the length at this point.
    */

    if (code < last_code) code = last_code;

    /* Paranoid check for integer overflow */

    if (OFLOW_MAX - *lengthptr < code - last_code)
      {
      *errorcodeptr = ERR20;
      goto FAILED;
      }

    *lengthptr += (int)(code - last_code);
    DPRINTF(("length=%d added %d c=%c (0x%x)\n", *lengthptr,
      (int)(code - last_code), c, c));

    /* If "previous" is set and it is not at the start of the work space, move
    it back to there, in order to avoid filling up the work space. Otherwise,
    if "previous" is NULL, reset the current code pointer to the start. */

    if (previous != NULL)
      {
      if (previous > orig_code)
        {
        memmove(orig_code, previous, IN_UCHARS(code - previous));
        code -= previous - orig_code;
        previous = orig_code;
        }
      }
    else code = orig_code;

    /* Remember where this code item starts so we can pick up the length
    next time round. */

    last_code = code;
    }

  /* In the real compile phase, just check the workspace used by the forward
  reference list. */

  else if (cd->hwm > cd->start_workspace + cd->workspace_size -
           WORK_SIZE_SAFETY_MARGIN)
    {
    *errorcodeptr = ERR52;
    goto FAILED;
    }

  /* If in \Q...\E, check for the end; if not, we have a literal */

  if (inescq && c != 0)
    {
    if (c == CHAR_BACKSLASH && ptr[1] == CHAR_E)
      {
      inescq = FALSE;
      ptr++;
      continue;
      }
    else
      {
      if (previous_callout != NULL)
        {
        if (lengthptr == NULL)  /* Don't attempt in pre-compile phase */
          complete_callout(previous_callout, ptr, cd);
        previous_callout = NULL;
        }
      if ((options & PCRE_AUTO_CALLOUT) != 0)
        {
        previous_callout = code;
        code = auto_callout(code, ptr, cd);
        }
      goto NORMAL_CHAR;
      }
    }

  /* Fill in length of a previous callout, except when the next thing is
  a quantifier. */

  is_quantifier =
    c == CHAR_ASTERISK || c == CHAR_PLUS || c == CHAR_QUESTION_MARK ||
    (c == CHAR_LEFT_CURLY_BRACKET && is_counted_repeat(ptr+1));

  if (!is_quantifier && previous_callout != NULL &&
       after_manual_callout-- <= 0)
    {
    if (lengthptr == NULL)      /* Don't attempt in pre-compile phase */
      complete_callout(previous_callout, ptr, cd);
    previous_callout = NULL;
    }

  /* In extended mode, skip white space and comments. */

  if ((options & PCRE_EXTENDED) != 0)
    {
    if (MAX_255(*ptr) && (cd->ctypes[c] & ctype_space) != 0) continue;
    if (c == CHAR_NUMBER_SIGN)
      {
      ptr++;
      while (*ptr != 0)
        {
        if (IS_NEWLINE(ptr)) { ptr += cd->nllen - 1; break; }
        ptr++;
#ifdef SUPPORT_UTF
        if (utf) FORWARDCHAR(ptr);
#endif
        }
      if (*ptr != 0) continue;

      /* Else fall through to handle end of string */
      c = 0;
      }
    }

  /* No auto callout for quantifiers. */

  if ((options & PCRE_AUTO_CALLOUT) != 0 && !is_quantifier)
    {
    previous_callout = code;
    code = auto_callout(code, ptr, cd);
    }

  switch(c)
    {
    /* ===================================================================*/
    case 0:                        /* The branch terminates at string end */
    case CHAR_VERTICAL_LINE:       /* or | or ) */
    case CHAR_RIGHT_PARENTHESIS:
    *firstcharptr = firstchar;
    *reqcharptr = reqchar;
    *codeptr = code;
    *ptrptr = ptr;
    if (lengthptr != NULL)
      {
      if (OFLOW_MAX - *lengthptr < code - last_code)
        {
        *errorcodeptr = ERR20;
        goto FAILED;
        }
      *lengthptr += (int)(code - last_code);   /* To include callout length */
      DPRINTF((">> end branch\n"));
      }
    return TRUE;


    /* ===================================================================*/
    /* Handle single-character metacharacters. In multiline mode, ^ disables
    the setting of any following char as a first character. */

    case CHAR_CIRCUMFLEX_ACCENT:
    previous = NULL;
    if ((options & PCRE_MULTILINE) != 0)
      {
      if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
      *code++ = OP_CIRCM;
      }
    else *code++ = OP_CIRC;
    break;

    case CHAR_DOLLAR_SIGN:
    previous = NULL;
    *code++ = ((options & PCRE_MULTILINE) != 0)? OP_DOLLM : OP_DOLL;
    break;

    /* There can never be a first char if '.' is first, whatever happens about
    repeats. The value of reqchar doesn't change either. */

    case CHAR_DOT:
    if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
    zerofirstchar = firstchar;
    zeroreqchar = reqchar;
    previous = code;
    *code++ = ((options & PCRE_DOTALL) != 0)? OP_ALLANY: OP_ANY;
    break;


    /* ===================================================================*/
    /* Character classes. If the included characters are all < 256, we build a
    32-byte bitmap of the permitted characters, except in the special case
    where there is only one such character. For negated classes, we build the
    map as usual, then invert it at the end. However, we use a different opcode
    so that data characters > 255 can be handled correctly.

    If the class contains characters outside the 0-255 range, a different
    opcode is compiled. It may optionally have a bit map for characters < 256,
    but those above are are explicitly listed afterwards. A flag byte tells
    whether the bitmap is present, and whether this is a negated class or not.

    In JavaScript compatibility mode, an isolated ']' causes an error. In
    default (Perl) mode, it is treated as a data character. */

    case CHAR_RIGHT_SQUARE_BRACKET:
    if ((cd->external_options & PCRE_JAVASCRIPT_COMPAT) != 0)
      {
      *errorcodeptr = ERR64;
      goto FAILED;
      }
    goto NORMAL_CHAR;

    case CHAR_LEFT_SQUARE_BRACKET:
    previous = code;

    /* PCRE supports POSIX class stuff inside a class. Perl gives an error if
    they are encountered at the top level, so we'll do that too. */

    if ((ptr[1] == CHAR_COLON || ptr[1] == CHAR_DOT ||
         ptr[1] == CHAR_EQUALS_SIGN) &&
        check_posix_syntax(ptr, &tempptr))
      {
      *errorcodeptr = (ptr[1] == CHAR_COLON)? ERR13 : ERR31;
      goto FAILED;
      }

    /* If the first character is '^', set the negation flag and skip it. Also,
    if the first few characters (either before or after ^) are \Q\E or \E we
    skip them too. This makes for compatibility with Perl. */

    negate_class = FALSE;
    for (;;)
      {
      c = *(++ptr);
      if (c == CHAR_BACKSLASH)
        {
        if (ptr[1] == CHAR_E)
          ptr++;
        else if (STRNCMP_UC_C8(ptr + 1, STR_Q STR_BACKSLASH STR_E, 3) == 0)
          ptr += 3;
        else
          break;
        }
      else if (!negate_class && c == CHAR_CIRCUMFLEX_ACCENT)
        negate_class = TRUE;
      else break;
      }

    /* Empty classes are allowed in JavaScript compatibility mode. Otherwise,
    an initial ']' is taken as a data character -- the code below handles
    that. In JS mode, [] must always fail, so generate OP_FAIL, whereas
    [^] must match any character, so generate OP_ALLANY. */

    if (c == CHAR_RIGHT_SQUARE_BRACKET &&
        (cd->external_options & PCRE_JAVASCRIPT_COMPAT) != 0)
      {
      *code++ = negate_class? OP_ALLANY : OP_FAIL;
      if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
      zerofirstchar = firstchar;
      break;
      }

    /* If a class contains a negative special such as \S, we need to flip the
    negation flag at the end, so that support for characters > 255 works
    correctly (they are all included in the class). */

    should_flip_negation = FALSE;

    /* For optimization purposes, we track some properties of the class.
    class_has_8bitchar will be non-zero, if the class contains at least one
    < 256 character. class_single_char will be 1 if the class contains only
    a single character. */

    class_has_8bitchar = 0;
    class_single_char = 0;

    /* Initialize the 32-char bit map to all zeros. We build the map in a
    temporary bit of memory, in case the class contains only 1 character (less
    than 256), because in that case the compiled code doesn't use the bit map.
    */

    memset(classbits, 0, 32 * sizeof(pcre_uint8));

#if defined SUPPORT_UTF || !defined COMPILE_PCRE8
    xclass = FALSE;                           /* No chars >= 256 */
    class_uchardata = code + LINK_SIZE + 2;   /* For UTF-8 items */
    class_uchardata_base = class_uchardata;   /* For resetting in pass 1 */
#endif

    /* Process characters until ] is reached. By writing this as a "do" it
    means that an initial ] is taken as a data character. At the start of the
    loop, c contains the first byte of the character. */

    if (c != 0) do
      {
      const pcre_uchar *oldptr;

#ifdef SUPPORT_UTF
      if (utf && HAS_EXTRALEN(c))
        {                           /* Braces are required because the */
        GETCHARLEN(c, ptr, ptr);    /* macro generates multiple statements */
        }
#endif

#if defined SUPPORT_UTF || !defined COMPILE_PCRE8
      /* In the pre-compile phase, accumulate the length of any extra
      data and reset the pointer. This is so that very large classes that
      contain a zillion > 255 characters no longer overwrite the work space
      (which is on the stack). */

      if (lengthptr != NULL)
        {
        *lengthptr += (int)(class_uchardata - class_uchardata_base);
        class_uchardata = class_uchardata_base;
        }
#endif

      /* Inside \Q...\E everything is literal except \E */

      if (inescq)
        {
        if (c == CHAR_BACKSLASH && ptr[1] == CHAR_E)  /* If we are at \E */
          {
          inescq = FALSE;                   /* Reset literal state */
          ptr++;                            /* Skip the 'E' */
          continue;                         /* Carry on with next */
          }
        goto CHECK_RANGE;                   /* Could be range if \E follows */
        }

      /* Handle POSIX class names. Perl allows a negation extension of the
      form [:^name:]. A square bracket that doesn't match the syntax is
      treated as a literal. We also recognize the POSIX constructions
      [.ch.] and [=ch=] ("collating elements") and fault them, as Perl
      5.6 and 5.8 do. */

      if (c == CHAR_LEFT_SQUARE_BRACKET &&
          (ptr[1] == CHAR_COLON || ptr[1] == CHAR_DOT ||
           ptr[1] == CHAR_EQUALS_SIGN) && check_posix_syntax(ptr, &tempptr))
        {
        BOOL local_negate = FALSE;
        int posix_class, taboffset, tabopt;
        register const pcre_uint8 *cbits = cd->cbits;
        pcre_uint8 pbits[32];

        if (ptr[1] != CHAR_COLON)
          {
          *errorcodeptr = ERR31;
          goto FAILED;
          }

        ptr += 2;
        if (*ptr == CHAR_CIRCUMFLEX_ACCENT)
          {
          local_negate = TRUE;
          should_flip_negation = TRUE;  /* Note negative special */
          ptr++;
          }

        posix_class = check_posix_name(ptr, (int)(tempptr - ptr));
        if (posix_class < 0)
          {
          *errorcodeptr = ERR30;
          goto FAILED;
          }

        /* If matching is caseless, upper and lower are converted to
        alpha. This relies on the fact that the class table starts with
        alpha, lower, upper as the first 3 entries. */

        if ((options & PCRE_CASELESS) != 0 && posix_class <= 2)
          posix_class = 0;

        /* When PCRE_UCP is set, some of the POSIX classes are converted to
        different escape sequences that use Unicode properties. */

#ifdef SUPPORT_UCP
        if ((options & PCRE_UCP) != 0)
          {
          int pc = posix_class + ((local_negate)? POSIX_SUBSIZE/2 : 0);
          if (posix_substitutes[pc] != NULL)
            {
            nestptr = tempptr + 1;
            ptr = posix_substitutes[pc] - 1;
            continue;
            }
          }
#endif
        /* In the non-UCP case, we build the bit map for the POSIX class in a
        chunk of local store because we may be adding and subtracting from it,
        and we don't want to subtract bits that may be in the main map already.
        At the end we or the result into the bit map that is being built. */

        posix_class *= 3;

        /* Copy in the first table (always present) */

        memcpy(pbits, cbits + posix_class_maps[posix_class],
          32 * sizeof(pcre_uint8));

        /* If there is a second table, add or remove it as required. */

        taboffset = posix_class_maps[posix_class + 1];
        tabopt = posix_class_maps[posix_class + 2];

        if (taboffset >= 0)
          {
          if (tabopt >= 0)
            for (c = 0; c < 32; c++) pbits[c] |= cbits[c + taboffset];
          else
            for (c = 0; c < 32; c++) pbits[c] &= ~cbits[c + taboffset];
          }

        /* Not see if we need to remove any special characters. An option
        value of 1 removes vertical space and 2 removes underscore. */

        if (tabopt < 0) tabopt = -tabopt;
        if (tabopt == 1) pbits[1] &= ~0x3c;
          else if (tabopt == 2) pbits[11] &= 0x7f;

        /* Add the POSIX table or its complement into the main table that is
        being built and we are done. */

        if (local_negate)
          for (c = 0; c < 32; c++) classbits[c] |= ~pbits[c];
        else
          for (c = 0; c < 32; c++) classbits[c] |= pbits[c];

        ptr = tempptr + 1;
        /* Every class contains at least one < 256 characters. */
        class_has_8bitchar = 1;
        /* Every class contains at least two characters. */
        class_single_char = 2;
        continue;    /* End of POSIX syntax handling */
        }

      /* Backslash may introduce a single character, or it may introduce one
      of the specials, which just set a flag. The sequence \b is a special
      case. Inside a class (and only there) it is treated as backspace. We
      assume that other escapes have more than one character in them, so
      speculatively set both class_has_8bitchar and class_single_char bigger
      than one. Unrecognized escapes fall through and are either treated
      as literal characters (by default), or are faulted if
      PCRE_EXTRA is set. */

      if (c == CHAR_BACKSLASH)
        {
        c = check_escape(&ptr, errorcodeptr, cd->bracount, options, TRUE);
        if (*errorcodeptr != 0) goto FAILED;

        if (-c == ESC_b) c = CHAR_BS;    /* \b is backspace in a class */
        else if (-c == ESC_N)            /* \N is not supported in a class */
          {
          *errorcodeptr = ERR71;
          goto FAILED;
          }
        else if (-c == ESC_Q)            /* Handle start of quoted string */
          {
          if (ptr[1] == CHAR_BACKSLASH && ptr[2] == CHAR_E)
            {
            ptr += 2; /* avoid empty string */
            }
          else inescq = TRUE;
          continue;
          }
        else if (-c == ESC_E) continue;  /* Ignore orphan \E */

        if (c < 0)
          {
          register const pcre_uint8 *cbits = cd->cbits;
          /* Every class contains at least two < 256 characters. */
          class_has_8bitchar++;
          /* Every class contains at least two characters. */
          class_single_char += 2;

          switch (-c)
            {
#ifdef SUPPORT_UCP
            case ESC_du:     /* These are the values given for \d etc */
            case ESC_DU:     /* when PCRE_UCP is set. We replace the */
            case ESC_wu:     /* escape sequence with an appropriate \p */
            case ESC_WU:     /* or \P to test Unicode properties instead */
            case ESC_su:     /* of the default ASCII testing. */
            case ESC_SU:
            nestptr = ptr;
            ptr = substitutes[-c - ESC_DU] - 1;  /* Just before substitute */
            class_has_8bitchar--;                /* Undo! */
            continue;
#endif
            case ESC_d:
            for (c = 0; c < 32; c++) classbits[c] |= cbits[c+cbit_digit];
            continue;

            case ESC_D:
            should_flip_negation = TRUE;
            for (c = 0; c < 32; c++) classbits[c] |= ~cbits[c+cbit_digit];
            continue;

            case ESC_w:
            for (c = 0; c < 32; c++) classbits[c] |= cbits[c+cbit_word];
            continue;

            case ESC_W:
            should_flip_negation = TRUE;
            for (c = 0; c < 32; c++) classbits[c] |= ~cbits[c+cbit_word];
            continue;

            /* Perl 5.004 onwards omits VT from \s, but we must preserve it
            if it was previously set by something earlier in the character
            class. */

            case ESC_s:
            classbits[0] |= cbits[cbit_space];
            classbits[1] |= cbits[cbit_space+1] & ~0x08;
            for (c = 2; c < 32; c++) classbits[c] |= cbits[c+cbit_space];
            continue;

            case ESC_S:
            should_flip_negation = TRUE;
            for (c = 0; c < 32; c++) classbits[c] |= ~cbits[c+cbit_space];
            classbits[1] |= 0x08;    /* Perl 5.004 onwards omits VT from \s */
            continue;

            case ESC_h:
            SETBIT(classbits, 0x09); /* VT */
            SETBIT(classbits, 0x20); /* SPACE */
            SETBIT(classbits, 0xa0); /* NSBP */
#ifndef COMPILE_PCRE8
            xclass = TRUE;
            *class_uchardata++ = XCL_SINGLE;
            *class_uchardata++ = 0x1680;
            *class_uchardata++ = XCL_SINGLE;
            *class_uchardata++ = 0x180e;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x2000;
            *class_uchardata++ = 0x200a;
            *class_uchardata++ = XCL_SINGLE;
            *class_uchardata++ = 0x202f;
            *class_uchardata++ = XCL_SINGLE;
            *class_uchardata++ = 0x205f;
            *class_uchardata++ = XCL_SINGLE;
            *class_uchardata++ = 0x3000;
#elif defined SUPPORT_UTF
            if (utf)
              {
              xclass = TRUE;
              *class_uchardata++ = XCL_SINGLE;
              class_uchardata += PRIV(ord2utf)(0x1680, class_uchardata);
              *class_uchardata++ = XCL_SINGLE;
              class_uchardata += PRIV(ord2utf)(0x180e, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x2000, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x200a, class_uchardata);
              *class_uchardata++ = XCL_SINGLE;
              class_uchardata += PRIV(ord2utf)(0x202f, class_uchardata);
              *class_uchardata++ = XCL_SINGLE;
              class_uchardata += PRIV(ord2utf)(0x205f, class_uchardata);
              *class_uchardata++ = XCL_SINGLE;
              class_uchardata += PRIV(ord2utf)(0x3000, class_uchardata);
              }
#endif
            continue;

            case ESC_H:
            for (c = 0; c < 32; c++)
              {
              int x = 0xff;
              switch (c)
                {
                case 0x09/8: x ^= 1 << (0x09%8); break;
                case 0x20/8: x ^= 1 << (0x20%8); break;
                case 0xa0/8: x ^= 1 << (0xa0%8); break;
                default: break;
                }
              classbits[c] |= x;
              }
#ifndef COMPILE_PCRE8
            xclass = TRUE;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x0100;
            *class_uchardata++ = 0x167f;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x1681;
            *class_uchardata++ = 0x180d;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x180f;
            *class_uchardata++ = 0x1fff;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x200b;
            *class_uchardata++ = 0x202e;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x2030;
            *class_uchardata++ = 0x205e;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x2060;
            *class_uchardata++ = 0x2fff;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x3001;
#ifdef SUPPORT_UTF
            if (utf)
              class_uchardata += PRIV(ord2utf)(0x10ffff, class_uchardata);
            else
#endif
              *class_uchardata++ = 0xffff;
#elif defined SUPPORT_UTF
            if (utf)
              {
              xclass = TRUE;
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x0100, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x167f, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x1681, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x180d, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x180f, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x1fff, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x200b, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x202e, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x2030, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x205e, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x2060, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x2fff, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x3001, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x10ffff, class_uchardata);
              }
#endif
            continue;

            case ESC_v:
            SETBIT(classbits, 0x0a); /* LF */
            SETBIT(classbits, 0x0b); /* VT */
            SETBIT(classbits, 0x0c); /* FF */
            SETBIT(classbits, 0x0d); /* CR */
            SETBIT(classbits, 0x85); /* NEL */
#ifndef COMPILE_PCRE8
            xclass = TRUE;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x2028;
            *class_uchardata++ = 0x2029;
#elif defined SUPPORT_UTF
            if (utf)
              {
              xclass = TRUE;
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x2028, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x2029, class_uchardata);
              }
#endif
            continue;

            case ESC_V:
            for (c = 0; c < 32; c++)
              {
              int x = 0xff;
              switch (c)
                {
                case 0x0a/8: x ^= 1 << (0x0a%8);
                             x ^= 1 << (0x0b%8);
                             x ^= 1 << (0x0c%8);
                             x ^= 1 << (0x0d%8);
                             break;
                case 0x85/8: x ^= 1 << (0x85%8); break;
                default: break;
                }
              classbits[c] |= x;
              }

#ifndef COMPILE_PCRE8
            xclass = TRUE;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x0100;
            *class_uchardata++ = 0x2027;
            *class_uchardata++ = XCL_RANGE;
            *class_uchardata++ = 0x202a;
#ifdef SUPPORT_UTF
            if (utf)
              class_uchardata += PRIV(ord2utf)(0x10ffff, class_uchardata);
            else
#endif
              *class_uchardata++ = 0xffff;
#elif defined SUPPORT_UTF
            if (utf)
              {
              xclass = TRUE;
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x0100, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x2027, class_uchardata);
              *class_uchardata++ = XCL_RANGE;
              class_uchardata += PRIV(ord2utf)(0x202a, class_uchardata);
              class_uchardata += PRIV(ord2utf)(0x10ffff, class_uchardata);
              }
#endif
            continue;

#ifdef SUPPORT_UCP
            case ESC_p:
            case ESC_P:
              {
              BOOL negated;
              int pdata;
              int ptype = get_ucp(&ptr, &negated, &pdata, errorcodeptr);
              if (ptype < 0) goto FAILED;
              xclass = TRUE;
              *class_uchardata++ = ((-c == ESC_p) != negated)?
                XCL_PROP : XCL_NOTPROP;
              *class_uchardata++ = ptype;
              *class_uchardata++ = pdata;
              class_has_8bitchar--;                /* Undo! */
              continue;
              }
#endif
            /* Unrecognized escapes are faulted if PCRE is running in its
            strict mode. By default, for compatibility with Perl, they are
            treated as literals. */

            default:
            if ((options & PCRE_EXTRA) != 0)
              {
              *errorcodeptr = ERR7;
              goto FAILED;
              }
            class_has_8bitchar--;    /* Undo the speculative increase. */
            class_single_char -= 2;  /* Undo the speculative increase. */
            c = *ptr;                /* Get the final character and fall through */
            break;
            }
          }

        /* Fall through if we have a single character (c >= 0). This may be
        greater than 256. */

        }   /* End of backslash handling */

      /* A single character may be followed by '-' to form a range. However,
      Perl does not permit ']' to be the end of the range. A '-' character
      at the end is treated as a literal. Perl ignores orphaned \E sequences
      entirely. The code for handling \Q and \E is messy. */

      CHECK_RANGE:
      while (ptr[1] == CHAR_BACKSLASH && ptr[2] == CHAR_E)
        {
        inescq = FALSE;
        ptr += 2;
        }

      oldptr = ptr;

      /* Remember \r or \n */

      if (c == CHAR_CR || c == CHAR_NL) cd->external_flags |= PCRE_HASCRORLF;

      /* Check for range */

      if (!inescq && ptr[1] == CHAR_MINUS)
        {
        int d;
        ptr += 2;
        while (*ptr == CHAR_BACKSLASH && ptr[1] == CHAR_E) ptr += 2;

        /* If we hit \Q (not followed by \E) at this point, go into escaped
        mode. */

        while (*ptr == CHAR_BACKSLASH && ptr[1] == CHAR_Q)
          {
          ptr += 2;
          if (*ptr == CHAR_BACKSLASH && ptr[1] == CHAR_E)
            { ptr += 2; continue; }
          inescq = TRUE;
          break;
          }

        if (*ptr == 0 || (!inescq && *ptr == CHAR_RIGHT_SQUARE_BRACKET))
          {
          ptr = oldptr;
          goto LONE_SINGLE_CHARACTER;
          }

#ifdef SUPPORT_UTF
        if (utf)
          {                           /* Braces are required because the */
          GETCHARLEN(d, ptr, ptr);    /* macro generates multiple statements */
          }
        else
#endif
        d = *ptr;  /* Not UTF-8 mode */

        /* The second part of a range can be a single-character escape, but
        not any of the other escapes. Perl 5.6 treats a hyphen as a literal
        in such circumstances. */

        if (!inescq && d == CHAR_BACKSLASH)
          {
          d = check_escape(&ptr, errorcodeptr, cd->bracount, options, TRUE);
          if (*errorcodeptr != 0) goto FAILED;

          /* \b is backspace; any other special means the '-' was literal */

          if (d < 0)
            {
            if (d == -ESC_b) d = CHAR_BS; else
              {
              ptr = oldptr;
              goto LONE_SINGLE_CHARACTER;  /* A few lines below */
              }
            }
          }

        /* Check that the two values are in the correct order. Optimize
        one-character ranges */

        if (d < c)
          {
          *errorcodeptr = ERR8;
          goto FAILED;
          }

        if (d == c) goto LONE_SINGLE_CHARACTER;  /* A few lines below */

        /* Remember \r or \n */

        if (d == CHAR_CR || d == CHAR_NL) cd->external_flags |= PCRE_HASCRORLF;

        /* Since we found a character range, single character optimizations
        cannot be done anymore. */
        class_single_char = 2;

        /* In UTF-8 mode, if the upper limit is > 255, or > 127 for caseless
        matching, we have to use an XCLASS with extra data items. Caseless
        matching for characters > 127 is available only if UCP support is
        available. */

#if defined SUPPORT_UTF && !(defined COMPILE_PCRE8)
        if ((d > 255) || (utf && ((options & PCRE_CASELESS) != 0 && d > 127)))
#elif defined  SUPPORT_UTF
        if (utf && (d > 255 || ((options & PCRE_CASELESS) != 0 && d > 127)))
#elif !(defined COMPILE_PCRE8)
        if (d > 255)
#endif
#if defined SUPPORT_UTF || !(defined COMPILE_PCRE8)
          {
          xclass = TRUE;

          /* With UCP support, we can find the other case equivalents of
          the relevant characters. There may be several ranges. Optimize how
          they fit with the basic range. */

#ifdef SUPPORT_UCP
#ifndef COMPILE_PCRE8
          if (utf && (options & PCRE_CASELESS) != 0)
#else
          if ((options & PCRE_CASELESS) != 0)
#endif
            {
            unsigned int occ, ocd;
            unsigned int cc = c;
            unsigned int origd = d;
            while (get_othercase_range(&cc, origd, &occ, &ocd))
              {
              if (occ >= (unsigned int)c &&
                  ocd <= (unsigned int)d)
                continue;                          /* Skip embedded ranges */

              if (occ < (unsigned int)c  &&
                  ocd >= (unsigned int)c - 1)      /* Extend the basic range */
                {                                  /* if there is overlap,   */
                c = occ;                           /* noting that if occ < c */
                continue;                          /* we can't have ocd > d  */
                }                                  /* because a subrange is  */
              if (ocd > (unsigned int)d &&
                  occ <= (unsigned int)d + 1)      /* always shorter than    */
                {                                  /* the basic range.       */
                d = ocd;
                continue;
                }

              if (occ == ocd)
                {
                *class_uchardata++ = XCL_SINGLE;
                }
              else
                {
                *class_uchardata++ = XCL_RANGE;
                class_uchardata += PRIV(ord2utf)(occ, class_uchardata);
                }
              class_uchardata += PRIV(ord2utf)(ocd, class_uchardata);
              }
            }
#endif  /* SUPPORT_UCP */

          /* Now record the original range, possibly modified for UCP caseless
          overlapping ranges. */

          *class_uchardata++ = XCL_RANGE;
#ifdef SUPPORT_UTF
#ifndef COMPILE_PCRE8
          if (utf)
            {
            class_uchardata += PRIV(ord2utf)(c, class_uchardata);
            class_uchardata += PRIV(ord2utf)(d, class_uchardata);
            }
          else
            {
            *class_uchardata++ = c;
            *class_uchardata++ = d;
            }
#else
          class_uchardata += PRIV(ord2utf)(c, class_uchardata);
          class_uchardata += PRIV(ord2utf)(d, class_uchardata);
#endif
#else /* SUPPORT_UTF */
          *class_uchardata++ = c;
          *class_uchardata++ = d;
#endif /* SUPPORT_UTF */

          /* With UCP support, we are done. Without UCP support, there is no
          caseless matching for UTF characters > 127; we can use the bit map
          for the smaller ones. As for 16 bit characters without UTF, we
          can still use  */

#ifdef SUPPORT_UCP
#ifndef COMPILE_PCRE8
          if (utf)
#endif
            continue;    /* With next character in the class */
#endif  /* SUPPORT_UCP */

#if defined SUPPORT_UTF && !defined(SUPPORT_UCP) && !(defined COMPILE_PCRE8)
          if (utf)
            {
            if ((options & PCRE_CASELESS) == 0 || c > 127) continue;
            /* Adjust upper limit and fall through to set up the map */
            d = 127;
            }
          else
            {
            if (c > 255) continue;
            /* Adjust upper limit and fall through to set up the map */
            d = 255;
            }
#elif defined SUPPORT_UTF && !defined(SUPPORT_UCP)
          if ((options & PCRE_CASELESS) == 0 || c > 127) continue;
          /* Adjust upper limit and fall through to set up the map */
          d = 127;
#else
          if (c > 255) continue;
          /* Adjust upper limit and fall through to set up the map */
          d = 255;
#endif  /* SUPPORT_UTF && !SUPPORT_UCP && !COMPILE_PCRE8 */
          }
#endif  /* SUPPORT_UTF || !COMPILE_PCRE8 */

        /* We use the bit map for 8 bit mode, or when the characters fall
        partially or entirely to [0-255] ([0-127] for UCP) ranges. */

        class_has_8bitchar = 1;

        /* We can save a bit of time by skipping this in the pre-compile. */

        if (lengthptr == NULL) for (; c <= d; c++)
          {
          classbits[c/8] |= (1 << (c&7));
          if ((options & PCRE_CASELESS) != 0)
            {
            int uc = cd->fcc[c]; /* flip case */
            classbits[uc/8] |= (1 << (uc&7));
            }
          }

        continue;   /* Go get the next char in the class */
        }

      /* Handle a lone single character - we can get here for a normal
      non-escape char, or after \ that introduces a single character or for an
      apparent range that isn't. */

      LONE_SINGLE_CHARACTER:

      /* Only the value of 1 matters for class_single_char. */
      if (class_single_char < 2) class_single_char++;

      /* If class_charcount is 1, we saw precisely one character. As long as
      there were no negated characters >= 128 and there was no use of \p or \P,
      in other words, no use of any XCLASS features, we can optimize.

      In UTF-8 mode, we can optimize the negative case only if there were no
      characters >= 128 because OP_NOT and the related opcodes like OP_NOTSTAR
      operate on single-bytes characters only. This is an historical hangover.
      Maybe one day we can tidy these opcodes to handle multi-byte characters.

      The optimization throws away the bit map. We turn the item into a
      1-character OP_CHAR[I] if it's positive, or OP_NOT[I] if it's negative.
      Note that OP_NOT[I] does not support multibyte characters. In the positive
      case, it can cause firstchar to be set. Otherwise, there can be no first
      char if this item is first, whatever repeat count may follow. In the case
      of reqchar, save the previous value for reinstating. */

#ifdef SUPPORT_UTF
      if (class_single_char == 1 && ptr[1] == CHAR_RIGHT_SQUARE_BRACKET
        && (!utf || !negate_class || c < (MAX_VALUE_FOR_SINGLE_CHAR + 1)))
#else
      if (class_single_char == 1 && ptr[1] == CHAR_RIGHT_SQUARE_BRACKET)
#endif
        {
        ptr++;
        zeroreqchar = reqchar;

        /* The OP_NOT[I] opcodes work on single characters only. */

        if (negate_class)
          {
          if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
          zerofirstchar = firstchar;
          *code++ = ((options & PCRE_CASELESS) != 0)? OP_NOTI: OP_NOT;
          *code++ = c;
          goto NOT_CHAR;
          }

        /* For a single, positive character, get the value into mcbuffer, and
        then we can handle this with the normal one-character code. */

#ifdef SUPPORT_UTF
        if (utf && c > MAX_VALUE_FOR_SINGLE_CHAR)
          mclength = PRIV(ord2utf)(c, mcbuffer);
        else
#endif
          {
          mcbuffer[0] = c;
          mclength = 1;
          }
        goto ONE_CHAR;
        }       /* End of 1-char optimization */

      /* Handle a character that cannot go in the bit map. */

#if defined SUPPORT_UTF && !(defined COMPILE_PCRE8)
      if ((c > 255) || (utf && ((options & PCRE_CASELESS) != 0 && c > 127)))
#elif defined SUPPORT_UTF
      if (utf && (c > 255 || ((options & PCRE_CASELESS) != 0 && c > 127)))
#elif !(defined COMPILE_PCRE8)
      if (c > 255)
#endif

#if defined SUPPORT_UTF || !(defined COMPILE_PCRE8)
        {
        xclass = TRUE;
        *class_uchardata++ = XCL_SINGLE;
#ifdef SUPPORT_UTF
#ifndef COMPILE_PCRE8
        /* In non 8 bit mode, we can get here even if we are not in UTF mode. */
        if (!utf)
          *class_uchardata++ = c;
        else
#endif
          class_uchardata += PRIV(ord2utf)(c, class_uchardata);
#else /* SUPPORT_UTF */
        *class_uchardata++ = c;
#endif /* SUPPORT_UTF */

#ifdef SUPPORT_UCP
#ifdef COMPILE_PCRE8
        if ((options & PCRE_CASELESS) != 0)
#else
        /* In non 8 bit mode, we can get here even if we are not in UTF mode. */
        if (utf && (options & PCRE_CASELESS) != 0)
#endif
          {
          unsigned int othercase;
          if ((int)(othercase = UCD_OTHERCASE(c)) != c)
            {
            *class_uchardata++ = XCL_SINGLE;
            class_uchardata += PRIV(ord2utf)(othercase, class_uchardata);
            }
          }
#endif  /* SUPPORT_UCP */

        }
      else
#endif  /* SUPPORT_UTF || COMPILE_PCRE16 */

      /* Handle a single-byte character */
        {
        class_has_8bitchar = 1;
        classbits[c/8] |= (1 << (c&7));
        if ((options & PCRE_CASELESS) != 0)
          {
          c = cd->fcc[c]; /* flip case */
          classbits[c/8] |= (1 << (c&7));
          }
        }
      }

    /* Loop until ']' reached. This "while" is the end of the "do" far above.
    If we are at the end of an internal nested string, revert to the outer
    string. */

    while (((c = *(++ptr)) != 0 ||
           (nestptr != NULL &&
             (ptr = nestptr, nestptr = NULL, c = *(++ptr)) != 0)) &&
           (c != CHAR_RIGHT_SQUARE_BRACKET || inescq));

    /* Check for missing terminating ']' */

    if (c == 0)
      {
      *errorcodeptr = ERR6;
      goto FAILED;
      }

    /* If this is the first thing in the branch, there can be no first char
    setting, whatever the repeat count. Any reqchar setting must remain
    unchanged after any kind of repeat. */

    if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
    zerofirstchar = firstchar;
    zeroreqchar = reqchar;

    /* If there are characters with values > 255, we have to compile an
    extended class, with its own opcode, unless there was a negated special
    such as \S in the class, and PCRE_UCP is not set, because in that case all
    characters > 255 are in the class, so any that were explicitly given as
    well can be ignored. If (when there are explicit characters > 255 that must
    be listed) there are no characters < 256, we can omit the bitmap in the
    actual compiled code. */

#ifdef SUPPORT_UTF
    if (xclass && (!should_flip_negation || (options & PCRE_UCP) != 0))
#elif !defined COMPILE_PCRE8
    if (xclass && !should_flip_negation)
#endif
#if defined SUPPORT_UTF || !defined COMPILE_PCRE8
      {
      *class_uchardata++ = XCL_END;    /* Marks the end of extra data */
      *code++ = OP_XCLASS;
      code += LINK_SIZE;
      *code = negate_class? XCL_NOT:0;

      /* If the map is required, move up the extra data to make room for it;
      otherwise just move the code pointer to the end of the extra data. */

      if (class_has_8bitchar > 0)
        {
        *code++ |= XCL_MAP;
        memmove(code + (32 / sizeof(pcre_uchar)), code,
          IN_UCHARS(class_uchardata - code));
        memcpy(code, classbits, 32);
        code = class_uchardata + (32 / sizeof(pcre_uchar));
        }
      else code = class_uchardata;

      /* Now fill in the complete length of the item */

      PUT(previous, 1, (int)(code - previous));
      break;   /* End of class handling */
      }
#endif

    /* If there are no characters > 255, or they are all to be included or
    excluded, set the opcode to OP_CLASS or OP_NCLASS, depending on whether the
    whole class was negated and whether there were negative specials such as \S
    (non-UCP) in the class. Then copy the 32-byte map into the code vector,
    negating it if necessary. */

    *code++ = (negate_class == should_flip_negation) ? OP_CLASS : OP_NCLASS;
    if (lengthptr == NULL)    /* Save time in the pre-compile phase */
      {
      if (negate_class)
        for (c = 0; c < 32; c++) classbits[c] = ~classbits[c];
      memcpy(code, classbits, 32);
      }
    code += 32 / sizeof(pcre_uchar);
    NOT_CHAR:
    break;


    /* ===================================================================*/
    /* Various kinds of repeat; '{' is not necessarily a quantifier, but this
    has been tested above. */

    case CHAR_LEFT_CURLY_BRACKET:
    if (!is_quantifier) goto NORMAL_CHAR;
    ptr = read_repeat_counts(ptr+1, &repeat_min, &repeat_max, errorcodeptr);
    if (*errorcodeptr != 0) goto FAILED;
    goto REPEAT;

    case CHAR_ASTERISK:
    repeat_min = 0;
    repeat_max = -1;
    goto REPEAT;

    case CHAR_PLUS:
    repeat_min = 1;
    repeat_max = -1;
    goto REPEAT;

    case CHAR_QUESTION_MARK:
    repeat_min = 0;
    repeat_max = 1;

    REPEAT:
    if (previous == NULL)
      {
      *errorcodeptr = ERR9;
      goto FAILED;
      }

    if (repeat_min == 0)
      {
      firstchar = zerofirstchar;    /* Adjust for zero repeat */
      reqchar = zeroreqchar;        /* Ditto */
      }

    /* Remember whether this is a variable length repeat */

    reqvary = (repeat_min == repeat_max)? 0 : REQ_VARY;

    op_type = 0;                    /* Default single-char op codes */
    possessive_quantifier = FALSE;  /* Default not possessive quantifier */

    /* Save start of previous item, in case we have to move it up in order to
    insert something before it. */

    tempcode = previous;

    /* If the next character is '+', we have a possessive quantifier. This
    implies greediness, whatever the setting of the PCRE_UNGREEDY option.
    If the next character is '?' this is a minimizing repeat, by default,
    but if PCRE_UNGREEDY is set, it works the other way round. We change the
    repeat type to the non-default. */

    if (ptr[1] == CHAR_PLUS)
      {
      repeat_type = 0;                  /* Force greedy */
      possessive_quantifier = TRUE;
      ptr++;
      }
    else if (ptr[1] == CHAR_QUESTION_MARK)
      {
      repeat_type = greedy_non_default;
      ptr++;
      }
    else repeat_type = greedy_default;

    /* If previous was a recursion call, wrap it in atomic brackets so that
    previous becomes the atomic group. All recursions were so wrapped in the
    past, but it no longer happens for non-repeated recursions. In fact, the
    repeated ones could be re-implemented independently so as not to need this,
    but for the moment we rely on the code for repeating groups. */

    if (*previous == OP_RECURSE)
      {
      memmove(previous + 1 + LINK_SIZE, previous, IN_UCHARS(1 + LINK_SIZE));
      *previous = OP_ONCE;
      PUT(previous, 1, 2 + 2*LINK_SIZE);
      previous[2 + 2*LINK_SIZE] = OP_KET;
      PUT(previous, 3 + 2*LINK_SIZE, 2 + 2*LINK_SIZE);
      code += 2 + 2 * LINK_SIZE;
      length_prevgroup = 3 + 3*LINK_SIZE;

      /* When actually compiling, we need to check whether this was a forward
      reference, and if so, adjust the offset. */

      if (lengthptr == NULL && cd->hwm >= cd->start_workspace + LINK_SIZE)
        {
        int offset = GET(cd->hwm, -LINK_SIZE);
        if (offset == previous + 1 - cd->start_code)
          PUT(cd->hwm, -LINK_SIZE, offset + 1 + LINK_SIZE);
        }
      }

    /* Now handle repetition for the different types of item. */

    /* If previous was a character match, abolish the item and generate a
    repeat item instead. If a char item has a minumum of more than one, ensure
    that it is set in reqchar - it might not be if a sequence such as x{3} is
    the first thing in a branch because the x will have gone into firstchar
    instead.  */

    if (*previous == OP_CHAR || *previous == OP_CHARI)
      {
      op_type = (*previous == OP_CHAR)? 0 : OP_STARI - OP_STAR;

      /* Deal with UTF characters that take up more than one character. It's
      easier to write this out separately than try to macrify it. Use c to
      hold the length of the character in bytes, plus UTF_LENGTH to flag that
      it's a length rather than a small character. */

#ifdef SUPPORT_UTF
      if (utf && NOT_FIRSTCHAR(code[-1]))
        {
        pcre_uchar *lastchar = code - 1;
        BACKCHAR(lastchar);
        c = (int)(code - lastchar);     /* Length of UTF-8 character */
        memcpy(utf_chars, lastchar, IN_UCHARS(c)); /* Save the char */
        c |= UTF_LENGTH;                /* Flag c as a length */
        }
      else
#endif /* SUPPORT_UTF */

      /* Handle the case of a single charater - either with no UTF support, or
      with UTF disabled, or for a single character UTF character. */
        {
        c = code[-1];
        if (repeat_min > 1) reqchar = c | req_caseopt | cd->req_varyopt;
        }

      /* If the repetition is unlimited, it pays to see if the next thing on
      the line is something that cannot possibly match this character. If so,
      automatically possessifying this item gains some performance in the case
      where the match fails. */

      if (!possessive_quantifier &&
          repeat_max < 0 &&
          check_auto_possessive(previous, utf, ptr + 1, options, cd))
        {
        repeat_type = 0;    /* Force greedy */
        possessive_quantifier = TRUE;
        }

      goto OUTPUT_SINGLE_REPEAT;   /* Code shared with single character types */
      }

    /* If previous was a single negated character ([^a] or similar), we use
    one of the special opcodes, replacing it. The code is shared with single-
    character repeats by setting opt_type to add a suitable offset into
    repeat_type. We can also test for auto-possessification. OP_NOT and OP_NOTI
    are currently used only for single-byte chars. */

    else if (*previous == OP_NOT || *previous == OP_NOTI)
      {
      op_type = ((*previous == OP_NOT)? OP_NOTSTAR : OP_NOTSTARI) - OP_STAR;
      c = previous[1];
      if (!possessive_quantifier &&
          repeat_max < 0 &&
          check_auto_possessive(previous, utf, ptr + 1, options, cd))
        {
        repeat_type = 0;    /* Force greedy */
        possessive_quantifier = TRUE;
        }
      goto OUTPUT_SINGLE_REPEAT;
      }

    /* If previous was a character type match (\d or similar), abolish it and
    create a suitable repeat item. The code is shared with single-character
    repeats by setting op_type to add a suitable offset into repeat_type. Note
    the the Unicode property types will be present only when SUPPORT_UCP is
    defined, but we don't wrap the little bits of code here because it just
    makes it horribly messy. */

    else if (*previous < OP_EODN)
      {
      pcre_uchar *oldcode;
      int prop_type, prop_value;
      op_type = OP_TYPESTAR - OP_STAR;  /* Use type opcodes */
      c = *previous;

      if (!possessive_quantifier &&
          repeat_max < 0 &&
          check_auto_possessive(previous, utf, ptr + 1, options, cd))
        {
        repeat_type = 0;    /* Force greedy */
        possessive_quantifier = TRUE;
        }

      OUTPUT_SINGLE_REPEAT:
      if (*previous == OP_PROP || *previous == OP_NOTPROP)
        {
        prop_type = previous[1];
        prop_value = previous[2];
        }
      else prop_type = prop_value = -1;

      oldcode = code;
      code = previous;                  /* Usually overwrite previous item */

      /* If the maximum is zero then the minimum must also be zero; Perl allows
      this case, so we do too - by simply omitting the item altogether. */

      if (repeat_max == 0) goto END_REPEAT;

      /*--------------------------------------------------------------------*/
      /* This code is obsolete from release 8.00; the restriction was finally
      removed: */

      /* All real repeats make it impossible to handle partial matching (maybe
      one day we will be able to remove this restriction). */

      /* if (repeat_max != 1) cd->external_flags |= PCRE_NOPARTIAL; */
      /*--------------------------------------------------------------------*/

      /* Combine the op_type with the repeat_type */

      repeat_type += op_type;

      /* A minimum of zero is handled either as the special case * or ?, or as
      an UPTO, with the maximum given. */

      if (repeat_min == 0)
        {
        if (repeat_max == -1) *code++ = OP_STAR + repeat_type;
          else if (repeat_max == 1) *code++ = OP_QUERY + repeat_type;
        else
          {
          *code++ = OP_UPTO + repeat_type;
          PUT2INC(code, 0, repeat_max);
          }
        }

      /* A repeat minimum of 1 is optimized into some special cases. If the
      maximum is unlimited, we use OP_PLUS. Otherwise, the original item is
      left in place and, if the maximum is greater than 1, we use OP_UPTO with
      one less than the maximum. */

      else if (repeat_min == 1)
        {
        if (repeat_max == -1)
          *code++ = OP_PLUS + repeat_type;
        else
          {
          code = oldcode;                 /* leave previous item in place */
          if (repeat_max == 1) goto END_REPEAT;
          *code++ = OP_UPTO + repeat_type;
          PUT2INC(code, 0, repeat_max - 1);
          }
        }

      /* The case {n,n} is just an EXACT, while the general case {n,m} is
      handled as an EXACT followed by an UPTO. */

      else
        {
        *code++ = OP_EXACT + op_type;  /* NB EXACT doesn't have repeat_type */
        PUT2INC(code, 0, repeat_min);

        /* If the maximum is unlimited, insert an OP_STAR. Before doing so,
        we have to insert the character for the previous code. For a repeated
        Unicode property match, there are two extra bytes that define the
        required property. In UTF-8 mode, long characters have their length in
        c, with the UTF_LENGTH bit as a flag. */

        if (repeat_max < 0)
          {
#ifdef SUPPORT_UTF
          if (utf && (c & UTF_LENGTH) != 0)
            {
            memcpy(code, utf_chars, IN_UCHARS(c & 7));
            code += c & 7;
            }
          else
#endif
            {
            *code++ = c;
            if (prop_type >= 0)
              {
              *code++ = prop_type;
              *code++ = prop_value;
              }
            }
          *code++ = OP_STAR + repeat_type;
          }

        /* Else insert an UPTO if the max is greater than the min, again
        preceded by the character, for the previously inserted code. If the
        UPTO is just for 1 instance, we can use QUERY instead. */

        else if (repeat_max != repeat_min)
          {
#ifdef SUPPORT_UTF
          if (utf && (c & UTF_LENGTH) != 0)
            {
            memcpy(code, utf_chars, IN_UCHARS(c & 7));
            code += c & 7;
            }
          else
#endif
          *code++ = c;
          if (prop_type >= 0)
            {
            *code++ = prop_type;
            *code++ = prop_value;
            }
          repeat_max -= repeat_min;

          if (repeat_max == 1)
            {
            *code++ = OP_QUERY + repeat_type;
            }
          else
            {
            *code++ = OP_UPTO + repeat_type;
            PUT2INC(code, 0, repeat_max);
            }
          }
        }

      /* The character or character type itself comes last in all cases. */

#ifdef SUPPORT_UTF
      if (utf && (c & UTF_LENGTH) != 0)
        {
        memcpy(code, utf_chars, IN_UCHARS(c & 7));
        code += c & 7;
        }
      else
#endif
      *code++ = c;

      /* For a repeated Unicode property match, there are two extra bytes that
      define the required property. */

#ifdef SUPPORT_UCP
      if (prop_type >= 0)
        {
        *code++ = prop_type;
        *code++ = prop_value;
        }
#endif
      }

    /* If previous was a character class or a back reference, we put the repeat
    stuff after it, but just skip the item if the repeat was {0,0}. */

    else if (*previous == OP_CLASS ||
             *previous == OP_NCLASS ||
#if defined SUPPORT_UTF || !defined COMPILE_PCRE8
             *previous == OP_XCLASS ||
#endif
             *previous == OP_REF ||
             *previous == OP_REFI)
      {
      if (repeat_max == 0)
        {
        code = previous;
        goto END_REPEAT;
        }

      /*--------------------------------------------------------------------*/
      /* This code is obsolete from release 8.00; the restriction was finally
      removed: */

      /* All real repeats make it impossible to handle partial matching (maybe
      one day we will be able to remove this restriction). */

      /* if (repeat_max != 1) cd->external_flags |= PCRE_NOPARTIAL; */
      /*--------------------------------------------------------------------*/

      if (repeat_min == 0 && repeat_max == -1)
        *code++ = OP_CRSTAR + repeat_type;
      else if (repeat_min == 1 && repeat_max == -1)
        *code++ = OP_CRPLUS + repeat_type;
      else if (repeat_min == 0 && repeat_max == 1)
        *code++ = OP_CRQUERY + repeat_type;
      else
        {
        *code++ = OP_CRRANGE + repeat_type;
        PUT2INC(code, 0, repeat_min);
        if (repeat_max == -1) repeat_max = 0;  /* 2-byte encoding for max */
        PUT2INC(code, 0, repeat_max);
        }
      }

    /* If previous was a bracket group, we may have to replicate it in certain
    cases. Note that at this point we can encounter only the "basic" bracket
    opcodes such as BRA and CBRA, as this is the place where they get converted
    into the more special varieties such as BRAPOS and SBRA. A test for >=
    OP_ASSERT and <= OP_COND includes ASSERT, ASSERT_NOT, ASSERTBACK,
    ASSERTBACK_NOT, ONCE, BRA, CBRA, and COND. Originally, PCRE did not allow
    repetition of assertions, but now it does, for Perl compatibility. */

    else if (*previous >= OP_ASSERT && *previous <= OP_COND)
      {
      register int i;
      int len = (int)(code - previous);
      pcre_uchar *bralink = NULL;
      pcre_uchar *brazeroptr = NULL;

      /* Repeating a DEFINE group is pointless, but Perl allows the syntax, so
      we just ignore the repeat. */

      if (*previous == OP_COND && previous[LINK_SIZE+1] == OP_DEF)
        goto END_REPEAT;

      /* There is no sense in actually repeating assertions. The only potential
      use of repetition is in cases when the assertion is optional. Therefore,
      if the minimum is greater than zero, just ignore the repeat. If the
      maximum is not not zero or one, set it to 1. */

      if (*previous < OP_ONCE)    /* Assertion */
        {
        if (repeat_min > 0) goto END_REPEAT;
        if (repeat_max < 0 || repeat_max > 1) repeat_max = 1;
        }

      /* The case of a zero minimum is special because of the need to stick
      OP_BRAZERO in front of it, and because the group appears once in the
      data, whereas in other cases it appears the minimum number of times. For
      this reason, it is simplest to treat this case separately, as otherwise
      the code gets far too messy. There are several special subcases when the
      minimum is zero. */

      if (repeat_min == 0)
        {
        /* If the maximum is also zero, we used to just omit the group from the
        output altogether, like this:

        ** if (repeat_max == 0)
        **   {
        **   code = previous;
        **   goto END_REPEAT;
        **   }

        However, that fails when a group or a subgroup within it is referenced
        as a subroutine from elsewhere in the pattern, so now we stick in
        OP_SKIPZERO in front of it so that it is skipped on execution. As we
        don't have a list of which groups are referenced, we cannot do this
        selectively.

        If the maximum is 1 or unlimited, we just have to stick in the BRAZERO
        and do no more at this point. However, we do need to adjust any
        OP_RECURSE calls inside the group that refer to the group itself or any
        internal or forward referenced group, because the offset is from the
        start of the whole regex. Temporarily terminate the pattern while doing
        this. */

        if (repeat_max <= 1)    /* Covers 0, 1, and unlimited */
          {
          *code = OP_END;
          adjust_recurse(previous, 1, utf, cd, save_hwm);
          memmove(previous + 1, previous, IN_UCHARS(len));
          code++;
          if (repeat_max == 0)
            {
            *previous++ = OP_SKIPZERO;
            goto END_REPEAT;
            }
          brazeroptr = previous;    /* Save for possessive optimizing */
          *previous++ = OP_BRAZERO + repeat_type;
          }

        /* If the maximum is greater than 1 and limited, we have to replicate
        in a nested fashion, sticking OP_BRAZERO before each set of brackets.
        The first one has to be handled carefully because it's the original
        copy, which has to be moved up. The remainder can be handled by code
        that is common with the non-zero minimum case below. We have to
        adjust the value or repeat_max, since one less copy is required. Once
        again, we may have to adjust any OP_RECURSE calls inside the group. */

        else
          {
          int offset;
          *code = OP_END;
          adjust_recurse(previous, 2 + LINK_SIZE, utf, cd, save_hwm);
          memmove(previous + 2 + LINK_SIZE, previous, IN_UCHARS(len));
          code += 2 + LINK_SIZE;
          *previous++ = OP_BRAZERO + repeat_type;
          *previous++ = OP_BRA;

          /* We chain together the bracket offset fields that have to be
          filled in later when the ends of the brackets are reached. */

          offset = (bralink == NULL)? 0 : (int)(previous - bralink);
          bralink = previous;
          PUTINC(previous, 0, offset);
          }

        repeat_max--;
        }

      /* If the minimum is greater than zero, replicate the group as many
      times as necessary, and adjust the maximum to the number of subsequent
      copies that we need. If we set a first char from the group, and didn't
      set a required char, copy the latter from the former. If there are any
      forward reference subroutine calls in the group, there will be entries on
      the workspace list; replicate these with an appropriate increment. */

      else
        {
        if (repeat_min > 1)
          {
          /* In the pre-compile phase, we don't actually do the replication. We
          just adjust the length as if we had. Do some paranoid checks for
          potential integer overflow. The INT64_OR_DOUBLE type is a 64-bit
          integer type when available, otherwise double. */

          if (lengthptr != NULL)
            {
            int delta = (repeat_min - 1)*length_prevgroup;
            if ((INT64_OR_DOUBLE)(repeat_min - 1)*
                  (INT64_OR_DOUBLE)length_prevgroup >
                    (INT64_OR_DOUBLE)INT_MAX ||
                OFLOW_MAX - *lengthptr < delta)
              {
              *errorcodeptr = ERR20;
              goto FAILED;
              }
            *lengthptr += delta;
            }

          /* This is compiling for real. If there is a set first byte for
          the group, and we have not yet set a "required byte", set it. Make
          sure there is enough workspace for copying forward references before
          doing the copy. */

          else
            {
            if (groupsetfirstchar && reqchar < 0) reqchar = firstchar;

            for (i = 1; i < repeat_min; i++)
              {
              pcre_uchar *hc;
              pcre_uchar *this_hwm = cd->hwm;
              memcpy(code, previous, IN_UCHARS(len));

              while (cd->hwm > cd->start_workspace + cd->workspace_size -
                     WORK_SIZE_SAFETY_MARGIN - (this_hwm - save_hwm))
                {
                int save_offset = (int)(save_hwm - cd->start_workspace);
                int this_offset = (int)(this_hwm - cd->start_workspace);
                *errorcodeptr = expand_workspace(cd);
                if (*errorcodeptr != 0) goto FAILED;
                save_hwm = (pcre_uchar *)cd->start_workspace + save_offset;
                this_hwm = (pcre_uchar *)cd->start_workspace + this_offset;
                }

              for (hc = save_hwm; hc < this_hwm; hc += LINK_SIZE)
                {
                PUT(cd->hwm, 0, GET(hc, 0) + len);
                cd->hwm += LINK_SIZE;
                }
              save_hwm = this_hwm;
              code += len;
              }
            }
          }

        if (repeat_max > 0) repeat_max -= repeat_min;
        }

      /* This code is common to both the zero and non-zero minimum cases. If
      the maximum is limited, it replicates the group in a nested fashion,
      remembering the bracket starts on a stack. In the case of a zero minimum,
      the first one was set up above. In all cases the repeat_max now specifies
      the number of additional copies needed. Again, we must remember to
      replicate entries on the forward reference list. */

      if (repeat_max >= 0)
        {
        /* In the pre-compile phase, we don't actually do the replication. We
        just adjust the length as if we had. For each repetition we must add 1
        to the length for BRAZERO and for all but the last repetition we must
        add 2 + 2*LINKSIZE to allow for the nesting that occurs. Do some
        paranoid checks to avoid integer overflow. The INT64_OR_DOUBLE type is
        a 64-bit integer type when available, otherwise double. */

        if (lengthptr != NULL && repeat_max > 0)
          {
          int delta = repeat_max * (length_prevgroup + 1 + 2 + 2*LINK_SIZE) -
                      2 - 2*LINK_SIZE;   /* Last one doesn't nest */
          if ((INT64_OR_DOUBLE)repeat_max *
                (INT64_OR_DOUBLE)(length_prevgroup + 1 + 2 + 2*LINK_SIZE)
                  > (INT64_OR_DOUBLE)INT_MAX ||
              OFLOW_MAX - *lengthptr < delta)
            {
            *errorcodeptr = ERR20;
            goto FAILED;
            }
          *lengthptr += delta;
          }

        /* This is compiling for real */

        else for (i = repeat_max - 1; i >= 0; i--)
          {
          pcre_uchar *hc;
          pcre_uchar *this_hwm = cd->hwm;

          *code++ = OP_BRAZERO + repeat_type;

          /* All but the final copy start a new nesting, maintaining the
          chain of brackets outstanding. */

          if (i != 0)
            {
            int offset;
            *code++ = OP_BRA;
            offset = (bralink == NULL)? 0 : (int)(code - bralink);
            bralink = code;
            PUTINC(code, 0, offset);
            }

          memcpy(code, previous, IN_UCHARS(len));

          /* Ensure there is enough workspace for forward references before
          copying them. */

          while (cd->hwm > cd->start_workspace + cd->workspace_size -
                 WORK_SIZE_SAFETY_MARGIN - (this_hwm - save_hwm))
            {
            int save_offset = (int)(save_hwm - cd->start_workspace);
            int this_offset = (int)(this_hwm - cd->start_workspace);
            *errorcodeptr = expand_workspace(cd);
            if (*errorcodeptr != 0) goto FAILED;
            save_hwm = (pcre_uchar *)cd->start_workspace + save_offset;
            this_hwm = (pcre_uchar *)cd->start_workspace + this_offset;
            }

          for (hc = save_hwm; hc < this_hwm; hc += LINK_SIZE)
            {
            PUT(cd->hwm, 0, GET(hc, 0) + len + ((i != 0)? 2+LINK_SIZE : 1));
            cd->hwm += LINK_SIZE;
            }
          save_hwm = this_hwm;
          code += len;
          }

        /* Now chain through the pending brackets, and fill in their length
        fields (which are holding the chain links pro tem). */

        while (bralink != NULL)
          {
          int oldlinkoffset;
          int offset = (int)(code - bralink + 1);
          pcre_uchar *bra = code - offset;
          oldlinkoffset = GET(bra, 1);
          bralink = (oldlinkoffset == 0)? NULL : bralink - oldlinkoffset;
          *code++ = OP_KET;
          PUTINC(code, 0, offset);
          PUT(bra, 1, offset);
          }
        }

      /* If the maximum is unlimited, set a repeater in the final copy. For
      ONCE brackets, that's all we need to do. However, possessively repeated
      ONCE brackets can be converted into non-capturing brackets, as the
      behaviour of (?:xx)++ is the same as (?>xx)++ and this saves having to
      deal with possessive ONCEs specially.

      Otherwise, when we are doing the actual compile phase, check to see
      whether this group is one that could match an empty string. If so,
      convert the initial operator to the S form (e.g. OP_BRA -> OP_SBRA) so
      that runtime checking can be done. [This check is also applied to ONCE
      groups at runtime, but in a different way.]

      Then, if the quantifier was possessive and the bracket is not a
      conditional, we convert the BRA code to the POS form, and the KET code to
      KETRPOS. (It turns out to be convenient at runtime to detect this kind of
      subpattern at both the start and at the end.) The use of special opcodes
      makes it possible to reduce greatly the stack usage in pcre_exec(). If
      the group is preceded by OP_BRAZERO, convert this to OP_BRAPOSZERO.

      Then, if the minimum number of matches is 1 or 0, cancel the possessive
      flag so that the default action below, of wrapping everything inside
      atomic brackets, does not happen. When the minimum is greater than 1,
      there will be earlier copies of the group, and so we still have to wrap
      the whole thing. */

      else
        {
        pcre_uchar *ketcode = code - 1 - LINK_SIZE;
        pcre_uchar *bracode = ketcode - GET(ketcode, 1);

        /* Convert possessive ONCE brackets to non-capturing */

        if ((*bracode == OP_ONCE || *bracode == OP_ONCE_NC) &&
            possessive_quantifier) *bracode = OP_BRA;

        /* For non-possessive ONCE brackets, all we need to do is to
        set the KET. */

        if (*bracode == OP_ONCE || *bracode == OP_ONCE_NC)
          *ketcode = OP_KETRMAX + repeat_type;

        /* Handle non-ONCE brackets and possessive ONCEs (which have been
        converted to non-capturing above). */

        else
          {
          /* In the compile phase, check for empty string matching. */

          if (lengthptr == NULL)
            {
            pcre_uchar *scode = bracode;
            do
              {
              if (could_be_empty_branch(scode, ketcode, utf, cd))
                {
                *bracode += OP_SBRA - OP_BRA;
                break;
                }
              scode += GET(scode, 1);
              }
            while (*scode == OP_ALT);
            }

          /* Handle possessive quantifiers. */

          if (possessive_quantifier)
            {
            /* For COND brackets, we wrap the whole thing in a possessively
            repeated non-capturing bracket, because we have not invented POS
            versions of the COND opcodes. Because we are moving code along, we
            must ensure that any pending recursive references are updated. */

            if (*bracode == OP_COND || *bracode == OP_SCOND)
              {
              int nlen = (int)(code - bracode);
              *code = OP_END;
              adjust_recurse(bracode, 1 + LINK_SIZE, utf, cd, save_hwm);
              memmove(bracode + 1 + LINK_SIZE, bracode, IN_UCHARS(nlen));
              code += 1 + LINK_SIZE;
              nlen += 1 + LINK_SIZE;
              *bracode = OP_BRAPOS;
              *code++ = OP_KETRPOS;
              PUTINC(code, 0, nlen);
              PUT(bracode, 1, nlen);
              }

            /* For non-COND brackets, we modify the BRA code and use KETRPOS. */

            else
              {
              *bracode += 1;              /* Switch to xxxPOS opcodes */
              *ketcode = OP_KETRPOS;
              }

            /* If the minimum is zero, mark it as possessive, then unset the
            possessive flag when the minimum is 0 or 1. */

            if (brazeroptr != NULL) *brazeroptr = OP_BRAPOSZERO;
            if (repeat_min < 2) possessive_quantifier = FALSE;
            }

          /* Non-possessive quantifier */

          else *ketcode = OP_KETRMAX + repeat_type;
          }
        }
      }

    /* If previous is OP_FAIL, it was generated by an empty class [] in
    JavaScript mode. The other ways in which OP_FAIL can be generated, that is
    by (*FAIL) or (?!) set previous to NULL, which gives a "nothing to repeat"
    error above. We can just ignore the repeat in JS case. */

    else if (*previous == OP_FAIL) goto END_REPEAT;

    /* Else there's some kind of shambles */

    else
      {
      *errorcodeptr = ERR11;
      goto FAILED;
      }

    /* If the character following a repeat is '+', or if certain optimization
    tests above succeeded, possessive_quantifier is TRUE. For some opcodes,
    there are special alternative opcodes for this case. For anything else, we
    wrap the entire repeated item inside OP_ONCE brackets. Logically, the '+'
    notation is just syntactic sugar, taken from Sun's Java package, but the
    special opcodes can optimize it.

    Some (but not all) possessively repeated subpatterns have already been
    completely handled in the code just above. For them, possessive_quantifier
    is always FALSE at this stage.

    Note that the repeated item starts at tempcode, not at previous, which
    might be the first part of a string whose (former) last char we repeated.

    Possessifying an 'exact' quantifier has no effect, so we can ignore it. But
    an 'upto' may follow. We skip over an 'exact' item, and then test the
    length of what remains before proceeding. */

    if (possessive_quantifier)
      {
      int len;

      if (*tempcode == OP_TYPEEXACT)
        tempcode += PRIV(OP_lengths)[*tempcode] +
          ((tempcode[1 + IMM2_SIZE] == OP_PROP
          || tempcode[1 + IMM2_SIZE] == OP_NOTPROP)? 2 : 0);

      else if (*tempcode == OP_EXACT || *tempcode == OP_NOTEXACT)
        {
        tempcode += PRIV(OP_lengths)[*tempcode];
#ifdef SUPPORT_UTF
        if (utf && HAS_EXTRALEN(tempcode[-1]))
          tempcode += GET_EXTRALEN(tempcode[-1]);
#endif
        }

      len = (int)(code - tempcode);
      if (len > 0) switch (*tempcode)
        {
        case OP_STAR:  *tempcode = OP_POSSTAR; break;
        case OP_PLUS:  *tempcode = OP_POSPLUS; break;
        case OP_QUERY: *tempcode = OP_POSQUERY; break;
        case OP_UPTO:  *tempcode = OP_POSUPTO; break;

        case OP_STARI:  *tempcode = OP_POSSTARI; break;
        case OP_PLUSI:  *tempcode = OP_POSPLUSI; break;
        case OP_QUERYI: *tempcode = OP_POSQUERYI; break;
        case OP_UPTOI:  *tempcode = OP_POSUPTOI; break;

        case OP_NOTSTAR:  *tempcode = OP_NOTPOSSTAR; break;
        case OP_NOTPLUS:  *tempcode = OP_NOTPOSPLUS; break;
        case OP_NOTQUERY: *tempcode = OP_NOTPOSQUERY; break;
        case OP_NOTUPTO:  *tempcode = OP_NOTPOSUPTO; break;

        case OP_NOTSTARI:  *tempcode = OP_NOTPOSSTARI; break;
        case OP_NOTPLUSI:  *tempcode = OP_NOTPOSPLUSI; break;
        case OP_NOTQUERYI: *tempcode = OP_NOTPOSQUERYI; break;
        case OP_NOTUPTOI:  *tempcode = OP_NOTPOSUPTOI; break;

        case OP_TYPESTAR:  *tempcode = OP_TYPEPOSSTAR; break;
        case OP_TYPEPLUS:  *tempcode = OP_TYPEPOSPLUS; break;
        case OP_TYPEQUERY: *tempcode = OP_TYPEPOSQUERY; break;
        case OP_TYPEUPTO:  *tempcode = OP_TYPEPOSUPTO; break;

        /* Because we are moving code along, we must ensure that any
        pending recursive references are updated. */

        default:
        *code = OP_END;
        adjust_recurse(tempcode, 1 + LINK_SIZE, utf, cd, save_hwm);
        memmove(tempcode + 1 + LINK_SIZE, tempcode, IN_UCHARS(len));
        code += 1 + LINK_SIZE;
        len += 1 + LINK_SIZE;
        tempcode[0] = OP_ONCE;
        *code++ = OP_KET;
        PUTINC(code, 0, len);
        PUT(tempcode, 1, len);
        break;
        }
      }

    /* In all case we no longer have a previous item. We also set the
    "follows varying string" flag for subsequently encountered reqchars if
    it isn't already set and we have just passed a varying length item. */

    END_REPEAT:
    previous = NULL;
    cd->req_varyopt |= reqvary;
    break;


    /* ===================================================================*/
    /* Start of nested parenthesized sub-expression, or comment or lookahead or
    lookbehind or option setting or condition or all the other extended
    parenthesis forms.  */

    case CHAR_LEFT_PARENTHESIS:
    newoptions = options;
    skipbytes = 0;
    bravalue = OP_CBRA;
    save_hwm = cd->hwm;
    reset_bracount = FALSE;

    /* First deal with various "verbs" that can be introduced by '*'. */

    ptr++;
    if (ptr[0] == CHAR_ASTERISK && (ptr[1] == ':'
         || (MAX_255(ptr[1]) && ((cd->ctypes[ptr[1]] & ctype_letter) != 0))))
      {
      int i, namelen;
      int arglen = 0;
      const char *vn = verbnames;
      const pcre_uchar *name = ptr + 1;
      const pcre_uchar *arg = NULL;
      previous = NULL;
      ptr++;
      while (MAX_255(*ptr) && (cd->ctypes[*ptr] & ctype_letter) != 0) ptr++;
      namelen = (int)(ptr - name);

      /* It appears that Perl allows any characters whatsoever, other than
      a closing parenthesis, to appear in arguments, so we no longer insist on
      letters, digits, and underscores. */

      if (*ptr == CHAR_COLON)
        {
        arg = ++ptr;
        while (*ptr != 0 && *ptr != CHAR_RIGHT_PARENTHESIS) ptr++;
        arglen = (int)(ptr - arg);
        }

      if (*ptr != CHAR_RIGHT_PARENTHESIS)
        {
        *errorcodeptr = ERR60;
        goto FAILED;
        }

      /* Scan the table of verb names */

      for (i = 0; i < verbcount; i++)
        {
        if (namelen == verbs[i].len &&
            STRNCMP_UC_C8(name, vn, namelen) == 0)
          {
          /* Check for open captures before ACCEPT and convert it to
          ASSERT_ACCEPT if in an assertion. */

          if (verbs[i].op == OP_ACCEPT)
            {
            open_capitem *oc;
            if (arglen != 0)
              {
              *errorcodeptr = ERR59;
              goto FAILED;
              }
            cd->had_accept = TRUE;
            for (oc = cd->open_caps; oc != NULL; oc = oc->next)
              {
              *code++ = OP_CLOSE;
              PUT2INC(code, 0, oc->number);
              }
            *code++ = (cd->assert_depth > 0)? OP_ASSERT_ACCEPT : OP_ACCEPT;

            /* Do not set firstchar after *ACCEPT */
            if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
            }

          /* Handle other cases with/without an argument */

          else if (arglen == 0)
            {
            if (verbs[i].op < 0)   /* Argument is mandatory */
              {
              *errorcodeptr = ERR66;
              goto FAILED;
              }
            *code = verbs[i].op;
            if (*code++ == OP_THEN) cd->external_flags |= PCRE_HASTHEN;
            }

          else
            {
            if (verbs[i].op_arg < 0)   /* Argument is forbidden */
              {
              *errorcodeptr = ERR59;
              goto FAILED;
              }
            *code = verbs[i].op_arg;
            if (*code++ == OP_THEN_ARG) cd->external_flags |= PCRE_HASTHEN;
            *code++ = arglen;
            memcpy(code, arg, IN_UCHARS(arglen));
            code += arglen;
            *code++ = 0;
            }

          break;  /* Found verb, exit loop */
          }

        vn += verbs[i].len + 1;
        }

      if (i < verbcount) continue;    /* Successfully handled a verb */
      *errorcodeptr = ERR60;          /* Verb not recognized */
      goto FAILED;
      }

    /* Deal with the extended parentheses; all are introduced by '?', and the
    appearance of any of them means that this is not a capturing group. */

    else if (*ptr == CHAR_QUESTION_MARK)
      {
      int i, set, unset, namelen;
      int *optset;
      const pcre_uchar *name;
      pcre_uchar *slot;

      switch (*(++ptr))
        {
        case CHAR_NUMBER_SIGN:                 /* Comment; skip to ket */
        ptr++;
        while (*ptr != 0 && *ptr != CHAR_RIGHT_PARENTHESIS) ptr++;
        if (*ptr == 0)
          {
          *errorcodeptr = ERR18;
          goto FAILED;
          }
        continue;


        /* ------------------------------------------------------------ */
        case CHAR_VERTICAL_LINE:  /* Reset capture count for each branch */
        reset_bracount = TRUE;
        /* Fall through */

        /* ------------------------------------------------------------ */
        case CHAR_COLON:          /* Non-capturing bracket */
        bravalue = OP_BRA;
        ptr++;
        break;


        /* ------------------------------------------------------------ */
        case CHAR_LEFT_PARENTHESIS:
        bravalue = OP_COND;       /* Conditional group */

        /* A condition can be an assertion, a number (referring to a numbered
        group), a name (referring to a named group), or 'R', referring to
        recursion. R<digits> and R&name are also permitted for recursion tests.

        There are several syntaxes for testing a named group: (?(name)) is used
        by Python; Perl 5.10 onwards uses (?(<name>) or (?('name')).

        There are two unfortunate ambiguities, caused by history. (a) 'R' can
        be the recursive thing or the name 'R' (and similarly for 'R' followed
        by digits), and (b) a number could be a name that consists of digits.
        In both cases, we look for a name first; if not found, we try the other
        cases. */

        /* For conditions that are assertions, check the syntax, and then exit
        the switch. This will take control down to where bracketed groups,
        including assertions, are processed. */

        if (ptr[1] == CHAR_QUESTION_MARK && (ptr[2] == CHAR_EQUALS_SIGN ||
            ptr[2] == CHAR_EXCLAMATION_MARK || ptr[2] == CHAR_LESS_THAN_SIGN))
          break;

        /* Most other conditions use OP_CREF (a couple change to OP_RREF
        below), and all need to skip 1+IMM2_SIZE bytes at the start of the group. */

        code[1+LINK_SIZE] = OP_CREF;
        skipbytes = 1+IMM2_SIZE;
        refsign = -1;

        /* Check for a test for recursion in a named group. */

        if (ptr[1] == CHAR_R && ptr[2] == CHAR_AMPERSAND)
          {
          terminator = -1;
          ptr += 2;
          code[1+LINK_SIZE] = OP_RREF;    /* Change the type of test */
          }

        /* Check for a test for a named group's having been set, using the Perl
        syntax (?(<name>) or (?('name') */

        else if (ptr[1] == CHAR_LESS_THAN_SIGN)
          {
          terminator = CHAR_GREATER_THAN_SIGN;
          ptr++;
          }
        else if (ptr[1] == CHAR_APOSTROPHE)
          {
          terminator = CHAR_APOSTROPHE;
          ptr++;
          }
        else
          {
          terminator = 0;
          if (ptr[1] == CHAR_MINUS || ptr[1] == CHAR_PLUS) refsign = *(++ptr);
          }

        /* We now expect to read a name; any thing else is an error */

        if (!MAX_255(ptr[1]) || (cd->ctypes[ptr[1]] & ctype_word) == 0)
          {
          ptr += 1;  /* To get the right offset */
          *errorcodeptr = ERR28;
          goto FAILED;
          }

        /* Read the name, but also get it as a number if it's all digits */

        recno = 0;
        name = ++ptr;
        while (MAX_255(*ptr) && (cd->ctypes[*ptr] & ctype_word) != 0)
          {
          if (recno >= 0)
            recno = (IS_DIGIT(*ptr))? recno * 10 + *ptr - CHAR_0 : -1;
          ptr++;
          }
        namelen = (int)(ptr - name);

        if ((terminator > 0 && *ptr++ != terminator) ||
            *ptr++ != CHAR_RIGHT_PARENTHESIS)
          {
          ptr--;      /* Error offset */
          *errorcodeptr = ERR26;
          goto FAILED;
          }

        /* Do no further checking in the pre-compile phase. */

        if (lengthptr != NULL) break;

        /* In the real compile we do the work of looking for the actual
        reference. If the string started with "+" or "-" we require the rest to
        be digits, in which case recno will be set. */

        if (refsign > 0)
          {
          if (recno <= 0)
            {
            *errorcodeptr = ERR58;
            goto FAILED;
            }
          recno = (refsign == CHAR_MINUS)?
            cd->bracount - recno + 1 : recno +cd->bracount;
          if (recno <= 0 || recno > cd->final_bracount)
            {
            *errorcodeptr = ERR15;
            goto FAILED;
            }
          PUT2(code, 2+LINK_SIZE, recno);
          break;
          }

        /* Otherwise (did not start with "+" or "-"), start by looking for the
        name. If we find a name, add one to the opcode to change OP_CREF or
        OP_RREF into OP_NCREF or OP_NRREF. These behave exactly the same,
        except they record that the reference was originally to a name. The
        information is used to check duplicate names. */

        slot = cd->name_table;
        for (i = 0; i < cd->names_found; i++)
          {
          if (STRNCMP_UC_UC(name, slot+IMM2_SIZE, namelen) == 0) break;
          slot += cd->name_entry_size;
          }

        /* Found a previous named subpattern */

        if (i < cd->names_found)
          {
          recno = GET2(slot, 0);
          PUT2(code, 2+LINK_SIZE, recno);
          code[1+LINK_SIZE]++;
          }

        /* Search the pattern for a forward reference */

        else if ((i = find_parens(cd, name, namelen,
                        (options & PCRE_EXTENDED) != 0, utf)) > 0)
          {
          PUT2(code, 2+LINK_SIZE, i);
          code[1+LINK_SIZE]++;
          }

        /* If terminator == 0 it means that the name followed directly after
        the opening parenthesis [e.g. (?(abc)...] and in this case there are
        some further alternatives to try. For the cases where terminator != 0
        [things like (?(<name>... or (?('name')... or (?(R&name)... ] we have
        now checked all the possibilities, so give an error. */

        else if (terminator != 0)
          {
          *errorcodeptr = ERR15;
          goto FAILED;
          }

        /* Check for (?(R) for recursion. Allow digits after R to specify a
        specific group number. */

        else if (*name == CHAR_R)
          {
          recno = 0;
          for (i = 1; i < namelen; i++)
            {
            if (!IS_DIGIT(name[i]))
              {
              *errorcodeptr = ERR15;
              goto FAILED;
              }
            recno = recno * 10 + name[i] - CHAR_0;
            }
          if (recno == 0) recno = RREF_ANY;
          code[1+LINK_SIZE] = OP_RREF;      /* Change test type */
          PUT2(code, 2+LINK_SIZE, recno);
          }

        /* Similarly, check for the (?(DEFINE) "condition", which is always
        false. */

        else if (namelen == 6 && STRNCMP_UC_C8(name, STRING_DEFINE, 6) == 0)
          {
          code[1+LINK_SIZE] = OP_DEF;
          skipbytes = 1;
          }

        /* Check for the "name" actually being a subpattern number. We are
        in the second pass here, so final_bracount is set. */

        else if (recno > 0 && recno <= cd->final_bracount)
          {
          PUT2(code, 2+LINK_SIZE, recno);
          }

        /* Either an unidentified subpattern, or a reference to (?(0) */

        else
          {
          *errorcodeptr = (recno == 0)? ERR35: ERR15;
          goto FAILED;
          }
        break;


        /* ------------------------------------------------------------ */
        case CHAR_EQUALS_SIGN:                 /* Positive lookahead */
        bravalue = OP_ASSERT;
        cd->assert_depth += 1;
        ptr++;
        break;


        /* ------------------------------------------------------------ */
        case CHAR_EXCLAMATION_MARK:            /* Negative lookahead */
        ptr++;
        if (*ptr == CHAR_RIGHT_PARENTHESIS)    /* Optimize (?!) */
          {
          *code++ = OP_FAIL;
          previous = NULL;
          continue;
          }
        bravalue = OP_ASSERT_NOT;
        cd->assert_depth += 1;
        break;


        /* ------------------------------------------------------------ */
        case CHAR_LESS_THAN_SIGN:              /* Lookbehind or named define */
        switch (ptr[1])
          {
          case CHAR_EQUALS_SIGN:               /* Positive lookbehind */
          bravalue = OP_ASSERTBACK;
          cd->assert_depth += 1;
          ptr += 2;
          break;

          case CHAR_EXCLAMATION_MARK:          /* Negative lookbehind */
          bravalue = OP_ASSERTBACK_NOT;
          cd->assert_depth += 1;
          ptr += 2;
          break;

          default:                /* Could be name define, else bad */
          if (MAX_255(ptr[1]) && (cd->ctypes[ptr[1]] & ctype_word) != 0)
            goto DEFINE_NAME;
          ptr++;                  /* Correct offset for error */
          *errorcodeptr = ERR24;
          goto FAILED;
          }
        break;


        /* ------------------------------------------------------------ */
        case CHAR_GREATER_THAN_SIGN:           /* One-time brackets */
        bravalue = OP_ONCE;
        ptr++;
        break;


        /* ------------------------------------------------------------ */
        case CHAR_C:                 /* Callout - may be followed by digits; */
        previous_callout = code;     /* Save for later completion */
        after_manual_callout = 1;    /* Skip one item before completing */
        *code++ = OP_CALLOUT;
          {
          int n = 0, arglen = 0;
		      // void *user_callout = NULL;
		      tempptr = ptr;
          ptr++;
          while(IS_DIGIT(*ptr))
            n = n * 10 + *ptr++ - CHAR_0;
          if (*ptr != CHAR_RIGHT_PARENTHESIS)
            {
			if (*ptr) /* Not end of string, so try to resolve it to a user-defined callout */
			  {
			  if (*ptr != ':') /* Treat (?C123Func) as 123Func() with callout_number == 0 */
				{
				n = 0;
				ptr = tempptr; /* Reset to address saved above */
				}
			  /* ptr now points at the character immediately preceding the function name - i.e. 'C' or ':' */
			  tempptr = ptr + 1;
			  while (*(++ptr) && *ptr != ')');
// 			  if (*ptr)
// #ifdef COMPILE_PCRE16
// 			    user_callout = pcre16_resolve_user_callout(tempptr, (int)(ptr - tempptr));
// #else
// 			    user_callout = pcre_resolve_user_callout(tempptr, (int)(ptr - tempptr));
// #endif
			  /* else fall through to return ERR39 */
			  }
			// if (user_callout == NULL) /* Unresolved user callout, or missing ')' */
			//   {
      //       *errorcodeptr = ERR39;
      //       goto FAILED;
      //   }
			// }
      if (!*ptr) /* or missing ')' */
              {
              *errorcodeptr = ERR39;
              goto FAILED;
              }
            }
      if (n > 255)
        {
        *errorcodeptr = ERR38;
        goto FAILED;
        }
      // *code++ = n;
		  // *((void **)code)++ = user_callout;
      //     PUT(code, 0, (int)(ptr - cd->start_pattern + 1)); /* Pattern offset */
      //     PUT(code, LINK_SIZE, 0);                          /* Default length */
      //     code += 2 * LINK_SIZE;
      //     }
      //   previous = NULL;
          arglen = (int)(ptr - tempptr);
          /* AutoHotkey: store the name the same way as (*MARK:NAME), and before the callout number */
          *code++ = arglen;
          memcpy(code, tempptr, IN_UCHARS(arglen));
          code += arglen;
          *code++ = 0; /* AutoHotkey: null-terminator for the name */
          *code++ = n;
          PUT(code, 0, (int)(ptr - cd->start_pattern + 1)); /* Pattern offset */
          PUT(code, LINK_SIZE, 0);                          /* Default length */
          code += 2 * LINK_SIZE;
          }
        previous = NULL;
        continue;


        /* ------------------------------------------------------------ */
        case CHAR_P:              /* Python-style named subpattern handling */
        if (*(++ptr) == CHAR_EQUALS_SIGN ||
            *ptr == CHAR_GREATER_THAN_SIGN)  /* Reference or recursion */
          {
          is_recurse = *ptr == CHAR_GREATER_THAN_SIGN;
          terminator = CHAR_RIGHT_PARENTHESIS;
          goto NAMED_REF_OR_RECURSE;
          }
        else if (*ptr != CHAR_LESS_THAN_SIGN)  /* Test for Python-style defn */
          {
          *errorcodeptr = ERR41;
          goto FAILED;
          }
        /* Fall through to handle (?P< as (?< is handled */


        /* ------------------------------------------------------------ */
        DEFINE_NAME:    /* Come here from (?< handling */
        case CHAR_APOSTROPHE:
          {
          terminator = (*ptr == CHAR_LESS_THAN_SIGN)?
            CHAR_GREATER_THAN_SIGN : CHAR_APOSTROPHE;
          name = ++ptr;

          while (MAX_255(*ptr) && (cd->ctypes[*ptr] & ctype_word) != 0) ptr++;
          namelen = (int)(ptr - name);

          /* In the pre-compile phase, just do a syntax check. */

          if (lengthptr != NULL)
            {
            if (*ptr != terminator)
              {
              *errorcodeptr = ERR42;
              goto FAILED;
              }
            if (cd->names_found >= MAX_NAME_COUNT)
              {
              *errorcodeptr = ERR49;
              goto FAILED;
              }
            if (namelen + IMM2_SIZE + 1 > cd->name_entry_size)
              {
              cd->name_entry_size = namelen + IMM2_SIZE + 1;
              if (namelen > MAX_NAME_SIZE)
                {
                *errorcodeptr = ERR48;
                goto FAILED;
                }
              }
            }

          /* In the real compile, create the entry in the table, maintaining
          alphabetical order. Duplicate names for different numbers are
          permitted only if PCRE_DUPNAMES is set. Duplicate names for the same
          number are always OK. (An existing number can be re-used if (?|
          appears in the pattern.) In either event, a duplicate name results in
          a duplicate entry in the table, even if the number is the same. This
          is because the number of names, and hence the table size, is computed
          in the pre-compile, and it affects various numbers and pointers which
          would all have to be modified, and the compiled code moved down, if
          duplicates with the same number were omitted from the table. This
          doesn't seem worth the hassle. However, *different* names for the
          same number are not permitted. */

          else
            {
            BOOL dupname = FALSE;
            slot = cd->name_table;

            for (i = 0; i < cd->names_found; i++)
              {
              int crc = memcmp(name, slot+IMM2_SIZE, IN_UCHARS(namelen));
              if (crc == 0)
                {
                if (slot[IMM2_SIZE+namelen] == 0)
                  {
                  if (GET2(slot, 0) != cd->bracount + 1 &&
                      (options & PCRE_DUPNAMES) == 0)
                    {
                    *errorcodeptr = ERR43;
                    goto FAILED;
                    }
                  else dupname = TRUE;
                  }
                else crc = -1;      /* Current name is a substring */
                }

              /* Make space in the table and break the loop for an earlier
              name. For a duplicate or later name, carry on. We do this for
              duplicates so that in the simple case (when ?(| is not used) they
              are in order of their numbers. */

              if (crc < 0)
                {
                memmove(slot + cd->name_entry_size, slot,
                  IN_UCHARS((cd->names_found - i) * cd->name_entry_size));
                break;
                }

              /* Continue the loop for a later or duplicate name */

              slot += cd->name_entry_size;
              }

            /* For non-duplicate names, check for a duplicate number before
            adding the new name. */

            if (!dupname)
              {
              pcre_uchar *cslot = cd->name_table;
              for (i = 0; i < cd->names_found; i++)
                {
                if (cslot != slot)
                  {
                  if (GET2(cslot, 0) == cd->bracount + 1)
                    {
                    *errorcodeptr = ERR65;
                    goto FAILED;
                    }
                  }
                else i--;
                cslot += cd->name_entry_size;
                }
              }

            PUT2(slot, 0, cd->bracount + 1);
            memcpy(slot + IMM2_SIZE, name, IN_UCHARS(namelen));
            slot[IMM2_SIZE + namelen] = 0;
            }
          }

        /* In both pre-compile and compile, count the number of names we've
        encountered. */

        cd->names_found++;
        ptr++;                    /* Move past > or ' */
        goto NUMBERED_GROUP;


        /* ------------------------------------------------------------ */
        case CHAR_AMPERSAND:            /* Perl recursion/subroutine syntax */
        terminator = CHAR_RIGHT_PARENTHESIS;
        is_recurse = TRUE;
        /* Fall through */

        /* We come here from the Python syntax above that handles both
        references (?P=name) and recursion (?P>name), as well as falling
        through from the Perl recursion syntax (?&name). We also come here from
        the Perl \k<name> or \k'name' back reference syntax and the \k{name}
        .NET syntax, and the Oniguruma \g<...> and \g'...' subroutine syntax. */

        NAMED_REF_OR_RECURSE:
        name = ++ptr;
        while (MAX_255(*ptr) && (cd->ctypes[*ptr] & ctype_word) != 0) ptr++;
        namelen = (int)(ptr - name);

        /* In the pre-compile phase, do a syntax check. We used to just set
        a dummy reference number, because it was not used in the first pass.
        However, with the change of recursive back references to be atomic,
        we have to look for the number so that this state can be identified, as
        otherwise the incorrect length is computed. If it's not a backwards
        reference, the dummy number will do. */

        if (lengthptr != NULL)
          {
          const pcre_uchar *temp;

          if (namelen == 0)
            {
            *errorcodeptr = ERR62;
            goto FAILED;
            }
          if (*ptr != terminator)
            {
            *errorcodeptr = ERR42;
            goto FAILED;
            }
          if (namelen > MAX_NAME_SIZE)
            {
            *errorcodeptr = ERR48;
            goto FAILED;
            }

          /* The name table does not exist in the first pass, so we cannot
          do a simple search as in the code below. Instead, we have to scan the
          pattern to find the number. It is important that we scan it only as
          far as we have got because the syntax of named subpatterns has not
          been checked for the rest of the pattern, and find_parens() assumes
          correct syntax. In any case, it's a waste of resources to scan
          further. We stop the scan at the current point by temporarily
          adjusting the value of cd->endpattern. */

          temp = cd->end_pattern;
          cd->end_pattern = ptr;
          recno = find_parens(cd, name, namelen,
            (options & PCRE_EXTENDED) != 0, utf);
          cd->end_pattern = temp;
          if (recno < 0) recno = 0;    /* Forward ref; set dummy number */
          }

        /* In the real compile, seek the name in the table. We check the name
        first, and then check that we have reached the end of the name in the
        table. That way, if the name that is longer than any in the table,
        the comparison will fail without reading beyond the table entry. */

        else
          {
          slot = cd->name_table;
          for (i = 0; i < cd->names_found; i++)
            {
            if (STRNCMP_UC_UC(name, slot+IMM2_SIZE, namelen) == 0 &&
                slot[IMM2_SIZE+namelen] == 0)
              break;
            slot += cd->name_entry_size;
            }

          if (i < cd->names_found)         /* Back reference */
            {
            recno = GET2(slot, 0);
            }
          else if ((recno =                /* Forward back reference */
                    find_parens(cd, name, namelen,
                      (options & PCRE_EXTENDED) != 0, utf)) <= 0)
            {
            *errorcodeptr = ERR15;
            goto FAILED;
            }
          }

        /* In both phases, we can now go to the code than handles numerical
        recursion or backreferences. */

        if (is_recurse) goto HANDLE_RECURSION;
          else goto HANDLE_REFERENCE;


        /* ------------------------------------------------------------ */
        case CHAR_R:              /* Recursion */
        ptr++;                    /* Same as (?0)      */
        /* Fall through */


        /* ------------------------------------------------------------ */
        case CHAR_MINUS: case CHAR_PLUS:  /* Recursion or subroutine */
        case CHAR_0: case CHAR_1: case CHAR_2: case CHAR_3: case CHAR_4:
        case CHAR_5: case CHAR_6: case CHAR_7: case CHAR_8: case CHAR_9:
          {
          const pcre_uchar *called;
          terminator = CHAR_RIGHT_PARENTHESIS;

          /* Come here from the \g<...> and \g'...' code (Oniguruma
          compatibility). However, the syntax has been checked to ensure that
          the ... are a (signed) number, so that neither ERR63 nor ERR29 will
          be called on this path, nor with the jump to OTHER_CHAR_AFTER_QUERY
          ever be taken. */

          HANDLE_NUMERICAL_RECURSION:

          if ((refsign = *ptr) == CHAR_PLUS)
            {
            ptr++;
            if (!IS_DIGIT(*ptr))
              {
              *errorcodeptr = ERR63;
              goto FAILED;
              }
            }
          else if (refsign == CHAR_MINUS)
            {
            if (!IS_DIGIT(ptr[1]))
              goto OTHER_CHAR_AFTER_QUERY;
            ptr++;
            }

          recno = 0;
          while(IS_DIGIT(*ptr))
            recno = recno * 10 + *ptr++ - CHAR_0;

          if (*ptr != terminator)
            {
            *errorcodeptr = ERR29;
            goto FAILED;
            }

          if (refsign == CHAR_MINUS)
            {
            if (recno == 0)
              {
              *errorcodeptr = ERR58;
              goto FAILED;
              }
            recno = cd->bracount - recno + 1;
            if (recno <= 0)
              {
              *errorcodeptr = ERR15;
              goto FAILED;
              }
            }
          else if (refsign == CHAR_PLUS)
            {
            if (recno == 0)
              {
              *errorcodeptr = ERR58;
              goto FAILED;
              }
            recno += cd->bracount;
            }

          /* Come here from code above that handles a named recursion */

          HANDLE_RECURSION:

          previous = code;
          called = cd->start_code;

          /* When we are actually compiling, find the bracket that is being
          referenced. Temporarily end the regex in case it doesn't exist before
          this point. If we end up with a forward reference, first check that
          the bracket does occur later so we can give the error (and position)
          now. Then remember this forward reference in the workspace so it can
          be filled in at the end. */

          if (lengthptr == NULL)
            {
            *code = OP_END;
            if (recno != 0)
              called = PRIV(find_bracket)(cd->start_code, utf, recno);

            /* Forward reference */

            if (called == NULL)
              {
              if (find_parens(cd, NULL, recno,
                    (options & PCRE_EXTENDED) != 0, utf) < 0)
                {
                *errorcodeptr = ERR15;
                goto FAILED;
                }

              /* Fudge the value of "called" so that when it is inserted as an
              offset below, what it actually inserted is the reference number
              of the group. Then remember the forward reference. */

              called = cd->start_code + recno;
              if (cd->hwm >= cd->start_workspace + cd->workspace_size -
                  WORK_SIZE_SAFETY_MARGIN)
                {
                *errorcodeptr = expand_workspace(cd);
                if (*errorcodeptr != 0) goto FAILED;
                }
              PUTINC(cd->hwm, 0, (int)(code + 1 - cd->start_code));
              }

            /* If not a forward reference, and the subpattern is still open,
            this is a recursive call. We check to see if this is a left
            recursion that could loop for ever, and diagnose that case. We
            must not, however, do this check if we are in a conditional
            subpattern because the condition might be testing for recursion in
            a pattern such as /(?(R)a+|(?R)b)/, which is perfectly valid.
            Forever loops are also detected at runtime, so those that occur in
            conditional subpatterns will be picked up then. */

            else if (GET(called, 1) == 0 && cond_depth <= 0 &&
                     could_be_empty(called, code, bcptr, utf, cd))
              {
              *errorcodeptr = ERR40;
              goto FAILED;
              }
            }

          /* Insert the recursion/subroutine item. It does not have a set first
          character (relevant if it is repeated, because it will then be
          wrapped with ONCE brackets). */

          *code = OP_RECURSE;
          PUT(code, 1, (int)(called - cd->start_code));
          code += 1 + LINK_SIZE;
          groupsetfirstchar = FALSE;
          }

        /* Can't determine a first byte now */

        if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
        continue;


        /* ------------------------------------------------------------ */
        default:              /* Other characters: check option setting */
        OTHER_CHAR_AFTER_QUERY:
        set = unset = 0;
        optset = &set;

        while (*ptr != CHAR_RIGHT_PARENTHESIS && *ptr != CHAR_COLON)
          {
          switch (*ptr++)
            {
            case CHAR_MINUS: optset = &unset; break;

            case CHAR_J:    /* Record that it changed in the external options */
            *optset |= PCRE_DUPNAMES;
            cd->external_flags |= PCRE_JCHANGED;
            break;

            case CHAR_i: *optset |= PCRE_CASELESS; break;
            case CHAR_m: *optset |= PCRE_MULTILINE; break;
            case CHAR_s: *optset |= PCRE_DOTALL; break;
            case CHAR_x: *optset |= PCRE_EXTENDED; break;
            case CHAR_U: *optset |= PCRE_UNGREEDY; break;
            case CHAR_X: *optset |= PCRE_EXTRA; break;

            default:  *errorcodeptr = ERR12;
                      ptr--;    /* Correct the offset */
                      goto FAILED;
            }
          }

        /* Set up the changed option bits, but don't change anything yet. */

        newoptions = (options | set) & (~unset);

        /* If the options ended with ')' this is not the start of a nested
        group with option changes, so the options change at this level. If this
        item is right at the start of the pattern, the options can be
        abstracted and made external in the pre-compile phase, and ignored in
        the compile phase. This can be helpful when matching -- for instance in
        caseless checking of required bytes.

        If the code pointer is not (cd->start_code + 1 + LINK_SIZE), we are
        definitely *not* at the start of the pattern because something has been
        compiled. In the pre-compile phase, however, the code pointer can have
        that value after the start, because it gets reset as code is discarded
        during the pre-compile. However, this can happen only at top level - if
        we are within parentheses, the starting BRA will still be present. At
        any parenthesis level, the length value can be used to test if anything
        has been compiled at that level. Thus, a test for both these conditions
        is necessary to ensure we correctly detect the start of the pattern in
        both phases.

        If we are not at the pattern start, reset the greedy defaults and the
        case value for firstchar and reqchar. */

        if (*ptr == CHAR_RIGHT_PARENTHESIS)
          {
          if (code == cd->start_code + 1 + LINK_SIZE &&
               (lengthptr == NULL || *lengthptr == 2 + 2*LINK_SIZE))
            {
            cd->external_options = newoptions;
            }
          else
            {
            greedy_default = ((newoptions & PCRE_UNGREEDY) != 0);
            greedy_non_default = greedy_default ^ 1;
            req_caseopt = ((newoptions & PCRE_CASELESS) != 0)? REQ_CASELESS:0;
            }

          /* Change options at this level, and pass them back for use
          in subsequent branches. */

          *optionsptr = options = newoptions;
          previous = NULL;       /* This item can't be repeated */
          continue;              /* It is complete */
          }

        /* If the options ended with ':' we are heading into a nested group
        with possible change of options. Such groups are non-capturing and are
        not assertions of any kind. All we need to do is skip over the ':';
        the newoptions value is handled below. */

        bravalue = OP_BRA;
        ptr++;
        }     /* End of switch for character following (? */
      }       /* End of (? handling */

    /* Opening parenthesis not followed by '*' or '?'. If PCRE_NO_AUTO_CAPTURE
    is set, all unadorned brackets become non-capturing and behave like (?:...)
    brackets. */

    else if ((options & PCRE_NO_AUTO_CAPTURE) != 0)
      {
      bravalue = OP_BRA;
      }

    /* Else we have a capturing group. */

    else
      {
      NUMBERED_GROUP:
      cd->bracount += 1;
      PUT2(code, 1+LINK_SIZE, cd->bracount);
      skipbytes = IMM2_SIZE;
      }

    /* Process nested bracketed regex. Assertions used not to be repeatable,
    but this was changed for Perl compatibility, so all kinds can now be
    repeated. We copy code into a non-register variable (tempcode) in order to
    be able to pass its address because some compilers complain otherwise. */

    previous = code;                      /* For handling repetition */
    *code = bravalue;
    tempcode = code;
    tempreqvary = cd->req_varyopt;        /* Save value before bracket */
    tempbracount = cd->bracount;          /* Save value before bracket */
    length_prevgroup = 0;                 /* Initialize for pre-compile phase */

    if (!compile_regex(
         newoptions,                      /* The complete new option state */
         &tempcode,                       /* Where to put code (updated) */
         &ptr,                            /* Input pointer (updated) */
         errorcodeptr,                    /* Where to put an error message */
         (bravalue == OP_ASSERTBACK ||
          bravalue == OP_ASSERTBACK_NOT), /* TRUE if back assert */
         reset_bracount,                  /* True if (?| group */
         skipbytes,                       /* Skip over bracket number */
         cond_depth +
           ((bravalue == OP_COND)?1:0),   /* Depth of condition subpatterns */
         &subfirstchar,                   /* For possible first char */
         &subreqchar,                     /* For possible last char */
         bcptr,                           /* Current branch chain */
         cd,                              /* Tables block */
         (lengthptr == NULL)? NULL :      /* Actual compile phase */
           &length_prevgroup              /* Pre-compile phase */
         ))
      goto FAILED;

    /* If this was an atomic group and there are no capturing groups within it,
    generate OP_ONCE_NC instead of OP_ONCE. */

    if (bravalue == OP_ONCE && cd->bracount <= tempbracount)
      *code = OP_ONCE_NC;

    if (bravalue >= OP_ASSERT && bravalue <= OP_ASSERTBACK_NOT)
      cd->assert_depth -= 1;

    /* At the end of compiling, code is still pointing to the start of the
    group, while tempcode has been updated to point past the end of the group.
    The pattern pointer (ptr) is on the bracket.

    If this is a conditional bracket, check that there are no more than
    two branches in the group, or just one if it's a DEFINE group. We do this
    in the real compile phase, not in the pre-pass, where the whole group may
    not be available. */

    if (bravalue == OP_COND && lengthptr == NULL)
      {
      pcre_uchar *tc = code;
      int condcount = 0;

      do {
         condcount++;
         tc += GET(tc,1);
         }
      while (*tc != OP_KET);

      /* A DEFINE group is never obeyed inline (the "condition" is always
      false). It must have only one branch. */

      if (code[LINK_SIZE+1] == OP_DEF)
        {
        if (condcount > 1)
          {
          *errorcodeptr = ERR54;
          goto FAILED;
          }
        bravalue = OP_DEF;   /* Just a flag to suppress char handling below */
        }

      /* A "normal" conditional group. If there is just one branch, we must not
      make use of its firstchar or reqchar, because this is equivalent to an
      empty second branch. */

      else
        {
        if (condcount > 2)
          {
          *errorcodeptr = ERR27;
          goto FAILED;
          }
        if (condcount == 1) subfirstchar = subreqchar = REQ_NONE;
        }
      }

    /* Error if hit end of pattern */

    if (*ptr != CHAR_RIGHT_PARENTHESIS)
      {
      *errorcodeptr = ERR14;
      goto FAILED;
      }

    /* In the pre-compile phase, update the length by the length of the group,
    less the brackets at either end. Then reduce the compiled code to just a
    set of non-capturing brackets so that it doesn't use much memory if it is
    duplicated by a quantifier.*/

    if (lengthptr != NULL)
      {
      if (OFLOW_MAX - *lengthptr < length_prevgroup - 2 - 2*LINK_SIZE)
        {
        *errorcodeptr = ERR20;
        goto FAILED;
        }
      *lengthptr += length_prevgroup - 2 - 2*LINK_SIZE;
      code++;   /* This already contains bravalue */
      PUTINC(code, 0, 1 + LINK_SIZE);
      *code++ = OP_KET;
      PUTINC(code, 0, 1 + LINK_SIZE);
      break;    /* No need to waste time with special character handling */
      }

    /* Otherwise update the main code pointer to the end of the group. */

    code = tempcode;

    /* For a DEFINE group, required and first character settings are not
    relevant. */

    if (bravalue == OP_DEF) break;

    /* Handle updating of the required and first characters for other types of
    group. Update for normal brackets of all kinds, and conditions with two
    branches (see code above). If the bracket is followed by a quantifier with
    zero repeat, we have to back off. Hence the definition of zeroreqchar and
    zerofirstchar outside the main loop so that they can be accessed for the
    back off. */

    zeroreqchar = reqchar;
    zerofirstchar = firstchar;
    groupsetfirstchar = FALSE;

    if (bravalue >= OP_ONCE)
      {
      /* If we have not yet set a firstchar in this branch, take it from the
      subpattern, remembering that it was set here so that a repeat of more
      than one can replicate it as reqchar if necessary. If the subpattern has
      no firstchar, set "none" for the whole branch. In both cases, a zero
      repeat forces firstchar to "none". */

      if (firstchar == REQ_UNSET)
        {
        if (subfirstchar >= 0)
          {
          firstchar = subfirstchar;
          groupsetfirstchar = TRUE;
          }
        else firstchar = REQ_NONE;
        zerofirstchar = REQ_NONE;
        }

      /* If firstchar was previously set, convert the subpattern's firstchar
      into reqchar if there wasn't one, using the vary flag that was in
      existence beforehand. */

      else if (subfirstchar >= 0 && subreqchar < 0)
        subreqchar = subfirstchar | tempreqvary;

      /* If the subpattern set a required byte (or set a first byte that isn't
      really the first byte - see above), set it. */

      if (subreqchar >= 0) reqchar = subreqchar;
      }

    /* For a forward assertion, we take the reqchar, if set. This can be
    helpful if the pattern that follows the assertion doesn't set a different
    char. For example, it's useful for /(?=abcde).+/. We can't set firstchar
    for an assertion, however because it leads to incorrect effect for patterns
    such as /(?=a)a.+/ when the "real" "a" would then become a reqchar instead
    of a firstchar. This is overcome by a scan at the end if there's no
    firstchar, looking for an asserted first char. */

    else if (bravalue == OP_ASSERT && subreqchar >= 0) reqchar = subreqchar;
    break;     /* End of processing '(' */


    /* ===================================================================*/
    /* Handle metasequences introduced by \. For ones like \d, the ESC_ values
    are arranged to be the negation of the corresponding OP_values in the
    default case when PCRE_UCP is not set. For the back references, the values
    are ESC_REF plus the reference number. Only back references and those types
    that consume a character may be repeated. We can test for values between
    ESC_b and ESC_Z for the latter; this may have to change if any new ones are
    ever created. */

    case CHAR_BACKSLASH:
    tempptr = ptr;
    c = check_escape(&ptr, errorcodeptr, cd->bracount, options, FALSE);
    if (*errorcodeptr != 0) goto FAILED;

    if (c < 0)
      {
      if (-c == ESC_Q)            /* Handle start of quoted string */
        {
        if (ptr[1] == CHAR_BACKSLASH && ptr[2] == CHAR_E)
          ptr += 2;               /* avoid empty string */
            else inescq = TRUE;
        continue;
        }

      if (-c == ESC_E) continue;  /* Perl ignores an orphan \E */

      /* For metasequences that actually match a character, we disable the
      setting of a first character if it hasn't already been set. */

      if (firstchar == REQ_UNSET && -c > ESC_b && -c < ESC_Z)
        firstchar = REQ_NONE;

      /* Set values to reset to if this is followed by a zero repeat. */

      zerofirstchar = firstchar;
      zeroreqchar = reqchar;

      /* \g<name> or \g'name' is a subroutine call by name and \g<n> or \g'n'
      is a subroutine call by number (Oniguruma syntax). In fact, the value
      -ESC_g is returned only for these cases. So we don't need to check for <
      or ' if the value is -ESC_g. For the Perl syntax \g{n} the value is
      -ESC_REF+n, and for the Perl syntax \g{name} the result is -ESC_k (as
      that is a synonym for a named back reference). */

      if (-c == ESC_g)
        {
        const pcre_uchar *p;
        save_hwm = cd->hwm;   /* Normally this is set when '(' is read */
        terminator = (*(++ptr) == CHAR_LESS_THAN_SIGN)?
          CHAR_GREATER_THAN_SIGN : CHAR_APOSTROPHE;

        /* These two statements stop the compiler for warning about possibly
        unset variables caused by the jump to HANDLE_NUMERICAL_RECURSION. In
        fact, because we actually check for a number below, the paths that
        would actually be in error are never taken. */

        skipbytes = 0;
        reset_bracount = FALSE;

        /* Test for a name */

        if (ptr[1] != CHAR_PLUS && ptr[1] != CHAR_MINUS)
          {
          BOOL is_a_number = TRUE;
          for (p = ptr + 1; *p != 0 && *p != terminator; p++)
            {
            if (!MAX_255(*p)) { is_a_number = FALSE; break; }
            if ((cd->ctypes[*p] & ctype_digit) == 0) is_a_number = FALSE;
            if ((cd->ctypes[*p] & ctype_word) == 0) break;
            }
          if (*p != terminator)
            {
            *errorcodeptr = ERR57;
            break;
            }
          if (is_a_number)
            {
            ptr++;
            goto HANDLE_NUMERICAL_RECURSION;
            }
          is_recurse = TRUE;
          goto NAMED_REF_OR_RECURSE;
          }

        /* Test a signed number in angle brackets or quotes. */

        p = ptr + 2;
        while (IS_DIGIT(*p)) p++;
        if (*p != terminator)
          {
          *errorcodeptr = ERR57;
          break;
          }
        ptr++;
        goto HANDLE_NUMERICAL_RECURSION;
        }

      /* \k<name> or \k'name' is a back reference by name (Perl syntax).
      We also support \k{name} (.NET syntax).  */

      if (-c == ESC_k)
        {
        if ((ptr[1] != CHAR_LESS_THAN_SIGN &&
          ptr[1] != CHAR_APOSTROPHE && ptr[1] != CHAR_LEFT_CURLY_BRACKET))
          {
          *errorcodeptr = ERR69;
          break;
          }
        is_recurse = FALSE;
        terminator = (*(++ptr) == CHAR_LESS_THAN_SIGN)?
          CHAR_GREATER_THAN_SIGN : (*ptr == CHAR_APOSTROPHE)?
          CHAR_APOSTROPHE : CHAR_RIGHT_CURLY_BRACKET;
        goto NAMED_REF_OR_RECURSE;
        }

      /* Back references are handled specially; must disable firstchar if
      not set to cope with cases like (?=(\w+))\1: which would otherwise set
      ':' later. */

      if (-c >= ESC_REF)
        {
        open_capitem *oc;
        recno = -c - ESC_REF;

        HANDLE_REFERENCE:    /* Come here from named backref handling */
        if (firstchar == REQ_UNSET) firstchar = REQ_NONE;
        previous = code;
        *code++ = ((options & PCRE_CASELESS) != 0)? OP_REFI : OP_REF;
        PUT2INC(code, 0, recno);
        cd->backref_map |= (recno < 32)? (1 << recno) : 1;
        if (recno > cd->top_backref) cd->top_backref = recno;

        /* Check to see if this back reference is recursive, that it, it
        is inside the group that it references. A flag is set so that the
        group can be made atomic. */

        for (oc = cd->open_caps; oc != NULL; oc = oc->next)
          {
          if (oc->number == recno)
            {
            oc->flag = TRUE;
            break;
            }
          }
        }

      /* So are Unicode property matches, if supported. */

#ifdef SUPPORT_UCP
      else if (-c == ESC_P || -c == ESC_p)
        {
        BOOL negated;
        int pdata;
        int ptype = get_ucp(&ptr, &negated, &pdata, errorcodeptr);
        if (ptype < 0) goto FAILED;
        previous = code;
        *code++ = ((-c == ESC_p) != negated)? OP_PROP : OP_NOTPROP;
        *code++ = ptype;
        *code++ = pdata;
        }
#else

      /* If Unicode properties are not supported, \X, \P, and \p are not
      allowed. */

      else if (-c == ESC_X || -c == ESC_P || -c == ESC_p)
        {
        *errorcodeptr = ERR45;
        goto FAILED;
        }
#endif

      /* For the rest (including \X when Unicode properties are supported), we
      can obtain the OP value by negating the escape value in the default
      situation when PCRE_UCP is not set. When it *is* set, we substitute
      Unicode property tests. */

      else
        {
#ifdef SUPPORT_UCP
        if (-c >= ESC_DU && -c <= ESC_wu)
          {
          nestptr = ptr + 1;                   /* Where to resume */
          ptr = substitutes[-c - ESC_DU] - 1;  /* Just before substitute */
          }
        else
#endif
        /* In non-UTF-8 mode, we turn \C into OP_ALLANY instead of OP_ANYBYTE
        so that it works in DFA mode and in lookbehinds. */

          {
          previous = (-c > ESC_b && -c < ESC_Z)? code : NULL;
          *code++ = (!utf && c == -ESC_C)? OP_ALLANY : -c;
          }
        }
      continue;
      }

    /* We have a data character whose value is in c. In UTF-8 mode it may have
    a value > 127. We set its representation in the length/buffer, and then
    handle it as a data character. */

#ifdef SUPPORT_UTF
    if (utf && c > MAX_VALUE_FOR_SINGLE_CHAR)
      mclength = PRIV(ord2utf)(c, mcbuffer);
    else
#endif

     {
     mcbuffer[0] = c;
     mclength = 1;
     }
    goto ONE_CHAR;


    /* ===================================================================*/
    /* Handle a literal character. It is guaranteed not to be whitespace or #
    when the extended flag is set. If we are in UTF-8 mode, it may be a
    multi-byte literal character. */

    default:
    NORMAL_CHAR:
    mclength = 1;
    mcbuffer[0] = c;

#ifdef SUPPORT_UTF
    if (utf && HAS_EXTRALEN(c))
      ACROSSCHAR(TRUE, ptr[1], mcbuffer[mclength++] = *(++ptr));
#endif

    /* At this point we have the character's bytes in mcbuffer, and the length
    in mclength. When not in UTF-8 mode, the length is always 1. */

    ONE_CHAR:
    previous = code;
    *code++ = ((options & PCRE_CASELESS) != 0)? OP_CHARI : OP_CHAR;
    for (c = 0; c < mclength; c++) *code++ = mcbuffer[c];

    /* Remember if \r or \n were seen */

    if (mcbuffer[0] == CHAR_CR || mcbuffer[0] == CHAR_NL)
      cd->external_flags |= PCRE_HASCRORLF;

    /* Set the first and required bytes appropriately. If no previous first
    byte, set it from this character, but revert to none on a zero repeat.
    Otherwise, leave the firstchar value alone, and don't change it on a zero
    repeat. */

    if (firstchar == REQ_UNSET)
      {
      zerofirstchar = REQ_NONE;
      zeroreqchar = reqchar;

      /* If the character is more than one byte long, we can set firstchar
      only if it is not to be matched caselessly. */

      if (mclength == 1 || req_caseopt == 0)
        {
        firstchar = mcbuffer[0] | req_caseopt;
        if (mclength != 1) reqchar = code[-1] | cd->req_varyopt;
        }
      else firstchar = reqchar = REQ_NONE;
      }

    /* firstchar was previously set; we can set reqchar only if the length is
    1 or the matching is caseful. */

    else
      {
      zerofirstchar = firstchar;
      zeroreqchar = reqchar;
      if (mclength == 1 || req_caseopt == 0)
        reqchar = code[-1] | req_caseopt | cd->req_varyopt;
      }

    break;            /* End of literal character handling */
    }
  }                   /* end of big loop */


/* Control never reaches here by falling through, only by a goto for all the
error states. Pass back the position in the pattern so that it can be displayed
to the user for diagnosing the error. */

FAILED:
*ptrptr = ptr;
return FALSE;
}




/*************************************************
*     Compile sequence of alternatives           *
*************************************************/

/* On entry, ptr is pointing past the bracket character, but on return it
points to the closing bracket, or vertical bar, or end of string. The code
variable is pointing at the byte into which the BRA operator has been stored.
This function is used during the pre-compile phase when we are trying to find
out the amount of memory needed, as well as during the real compile phase. The
value of lengthptr distinguishes the two phases.

Arguments:
  options        option bits, including any changes for this subpattern
  codeptr        -> the address of the current code pointer
  ptrptr         -> the address of the current pattern pointer
  errorcodeptr   -> pointer to error code variable
  lookbehind     TRUE if this is a lookbehind assertion
  reset_bracount TRUE to reset the count for each branch
  skipbytes      skip this many bytes at start (for brackets and OP_COND)
  cond_depth     depth of nesting for conditional subpatterns
  firstcharptr   place to put the first required character, or a negative number
  reqcharptr     place to put the last required character, or a negative number
  bcptr          pointer to the chain of currently open branches
  cd             points to the data block with tables pointers etc.
  lengthptr      NULL during the real compile phase
                 points to length accumulator during pre-compile phase

Returns:         TRUE on success
*/

static BOOL
compile_regex(int options, pcre_uchar **codeptr, const pcre_uchar **ptrptr,
  int *errorcodeptr, BOOL lookbehind, BOOL reset_bracount, int skipbytes,
  int cond_depth, pcre_int32 *firstcharptr, pcre_int32 *reqcharptr,
  branch_chain *bcptr, compile_data *cd, int *lengthptr)
{
const pcre_uchar *ptr = *ptrptr;
pcre_uchar *code = *codeptr;
pcre_uchar *last_branch = code;
pcre_uchar *start_bracket = code;
pcre_uchar *reverse_count = NULL;
open_capitem capitem;
int capnumber = 0;
pcre_int32 firstchar, reqchar;
pcre_int32 branchfirstchar, branchreqchar;
int length;
int orig_bracount;
int max_bracount;
branch_chain bc;

bc.outer = bcptr;
bc.current_branch = code;

firstchar = reqchar = REQ_UNSET;

/* Accumulate the length for use in the pre-compile phase. Start with the
length of the BRA and KET and any extra bytes that are required at the
beginning. We accumulate in a local variable to save frequent testing of
lenthptr for NULL. We cannot do this by looking at the value of code at the
start and end of each alternative, because compiled items are discarded during
the pre-compile phase so that the work space is not exceeded. */

length = 2 + 2*LINK_SIZE + skipbytes;

/* WARNING: If the above line is changed for any reason, you must also change
the code that abstracts option settings at the start of the pattern and makes
them global. It tests the value of length for (2 + 2*LINK_SIZE) in the
pre-compile phase to find out whether anything has yet been compiled or not. */

/* If this is a capturing subpattern, add to the chain of open capturing items
so that we can detect them if (*ACCEPT) is encountered. This is also used to
detect groups that contain recursive back references to themselves. Note that
only OP_CBRA need be tested here; changing this opcode to one of its variants,
e.g. OP_SCBRAPOS, happens later, after the group has been compiled. */

if (*code == OP_CBRA)
  {
  capnumber = GET2(code, 1 + LINK_SIZE);
  capitem.number = capnumber;
  capitem.next = cd->open_caps;
  capitem.flag = FALSE;
  cd->open_caps = &capitem;
  }

/* Offset is set zero to mark that this bracket is still open */

PUT(code, 1, 0);
code += 1 + LINK_SIZE + skipbytes;

/* Loop for each alternative branch */

orig_bracount = max_bracount = cd->bracount;
for (;;)
  {
  /* For a (?| group, reset the capturing bracket count so that each branch
  uses the same numbers. */

  if (reset_bracount) cd->bracount = orig_bracount;

  /* Set up dummy OP_REVERSE if lookbehind assertion */

  if (lookbehind)
    {
    *code++ = OP_REVERSE;
    reverse_count = code;
    PUTINC(code, 0, 0);
    length += 1 + LINK_SIZE;
    }

  /* Now compile the branch; in the pre-compile phase its length gets added
  into the length. */

  if (!compile_branch(&options, &code, &ptr, errorcodeptr, &branchfirstchar,
        &branchreqchar, &bc, cond_depth, cd,
        (lengthptr == NULL)? NULL : &length))
    {
    *ptrptr = ptr;
    return FALSE;
    }

  /* Keep the highest bracket count in case (?| was used and some branch
  has fewer than the rest. */

  if (cd->bracount > max_bracount) max_bracount = cd->bracount;

  /* In the real compile phase, there is some post-processing to be done. */

  if (lengthptr == NULL)
    {
    /* If this is the first branch, the firstchar and reqchar values for the
    branch become the values for the regex. */

    if (*last_branch != OP_ALT)
      {
      firstchar = branchfirstchar;
      reqchar = branchreqchar;
      }

    /* If this is not the first branch, the first char and reqchar have to
    match the values from all the previous branches, except that if the
    previous value for reqchar didn't have REQ_VARY set, it can still match,
    and we set REQ_VARY for the regex. */

    else
      {
      /* If we previously had a firstchar, but it doesn't match the new branch,
      we have to abandon the firstchar for the regex, but if there was
      previously no reqchar, it takes on the value of the old firstchar. */

      if (firstchar >= 0 && firstchar != branchfirstchar)
        {
        if (reqchar < 0) reqchar = firstchar;
        firstchar = REQ_NONE;
        }

      /* If we (now or from before) have no firstchar, a firstchar from the
      branch becomes a reqchar if there isn't a branch reqchar. */

      if (firstchar < 0 && branchfirstchar >= 0 && branchreqchar < 0)
          branchreqchar = branchfirstchar;

      /* Now ensure that the reqchars match */

      if ((reqchar & ~REQ_VARY) != (branchreqchar & ~REQ_VARY))
        reqchar = REQ_NONE;
      else reqchar |= branchreqchar;   /* To "or" REQ_VARY */
      }

    /* If lookbehind, check that this branch matches a fixed-length string, and
    put the length into the OP_REVERSE item. Temporarily mark the end of the
    branch with OP_END. If the branch contains OP_RECURSE, the result is -3
    because there may be forward references that we can't check here. Set a
    flag to cause another lookbehind check at the end. Why not do it all at the
    end? Because common, erroneous checks are picked up here and the offset of
    the problem can be shown. */

    if (lookbehind)
      {
      int fixed_length;
      *code = OP_END;
      fixed_length = find_fixedlength(last_branch,  (options & PCRE_UTF8) != 0,
        FALSE, cd);
      DPRINTF(("fixed length = %d\n", fixed_length));
      if (fixed_length == -3)
        {
        cd->check_lookbehind = TRUE;
        }
      else if (fixed_length < 0)
        {
        *errorcodeptr = (fixed_length == -2)? ERR36 :
                        (fixed_length == -4)? ERR70: ERR25;
        *ptrptr = ptr;
        return FALSE;
        }
      else { PUT(reverse_count, 0, fixed_length); }
      }
    }

  /* Reached end of expression, either ')' or end of pattern. In the real
  compile phase, go back through the alternative branches and reverse the chain
  of offsets, with the field in the BRA item now becoming an offset to the
  first alternative. If there are no alternatives, it points to the end of the
  group. The length in the terminating ket is always the length of the whole
  bracketed item. Return leaving the pointer at the terminating char. */

  if (*ptr != CHAR_VERTICAL_LINE)
    {
    if (lengthptr == NULL)
      {
      int branch_length = (int)(code - last_branch);
      do
        {
        int prev_length = GET(last_branch, 1);
        PUT(last_branch, 1, branch_length);
        branch_length = prev_length;
        last_branch -= branch_length;
        }
      while (branch_length > 0);
      }

    /* Fill in the ket */

    *code = OP_KET;
    PUT(code, 1, (int)(code - start_bracket));
    code += 1 + LINK_SIZE;

    /* If it was a capturing subpattern, check to see if it contained any
    recursive back references. If so, we must wrap it in atomic brackets.
    In any event, remove the block from the chain. */

    if (capnumber > 0)
      {
      if (cd->open_caps->flag)
        {
        memmove(start_bracket + 1 + LINK_SIZE, start_bracket,
          IN_UCHARS(code - start_bracket));
        *start_bracket = OP_ONCE;
        code += 1 + LINK_SIZE;
        PUT(start_bracket, 1, (int)(code - start_bracket));
        *code = OP_KET;
        PUT(code, 1, (int)(code - start_bracket));
        code += 1 + LINK_SIZE;
        length += 2 + 2*LINK_SIZE;
        }
      cd->open_caps = cd->open_caps->next;
      }

    /* Retain the highest bracket number, in case resetting was used. */

    cd->bracount = max_bracount;

    /* Set values to pass back */

    *codeptr = code;
    *ptrptr = ptr;
    *firstcharptr = firstchar;
    *reqcharptr = reqchar;
    if (lengthptr != NULL)
      {
      if (OFLOW_MAX - *lengthptr < length)
        {
        *errorcodeptr = ERR20;
        return FALSE;
        }
      *lengthptr += length;
      }
    return TRUE;
    }

  /* Another branch follows. In the pre-compile phase, we can move the code
  pointer back to where it was for the start of the first branch. (That is,
  pretend that each branch is the only one.)

  In the real compile phase, insert an ALT node. Its length field points back
  to the previous branch while the bracket remains open. At the end the chain
  is reversed. It's done like this so that the start of the bracket has a
  zero offset until it is closed, making it possible to detect recursion. */

  if (lengthptr != NULL)
    {
    code = *codeptr + 1 + LINK_SIZE + skipbytes;
    length += 1 + LINK_SIZE;
    }
  else
    {
    *code = OP_ALT;
    PUT(code, 1, (int)(code - last_branch));
    bc.current_branch = last_branch = code;
    code += 1 + LINK_SIZE;
    }

  ptr++;
  }
/* Control never reaches here */
}




/*************************************************
*          Check for anchored expression         *
*************************************************/

/* Try to find out if this is an anchored regular expression. Consider each
alternative branch. If they all start with OP_SOD or OP_CIRC, or with a bracket
all of whose alternatives start with OP_SOD or OP_CIRC (recurse ad lib), then
it's anchored. However, if this is a multiline pattern, then only OP_SOD will
be found, because ^ generates OP_CIRCM in that mode.

We can also consider a regex to be anchored if OP_SOM starts all its branches.
This is the code for \G, which means "match at start of match position, taking
into account the match offset".

A branch is also implicitly anchored if it starts with .* and DOTALL is set,
because that will try the rest of the pattern at all possible matching points,
so there is no point trying again.... er ....

.... except when the .* appears inside capturing parentheses, and there is a
subsequent back reference to those parentheses. We haven't enough information
to catch that case precisely.

At first, the best we could do was to detect when .* was in capturing brackets
and the highest back reference was greater than or equal to that level.
However, by keeping a bitmap of the first 31 back references, we can catch some
of the more common cases more precisely.

Arguments:
  code           points to start of expression (the bracket)
  bracket_map    a bitmap of which brackets we are inside while testing; this
                  handles up to substring 31; after that we just have to take
                  the less precise approach
  backref_map    the back reference bitmap

Returns:     TRUE or FALSE
*/

static BOOL
is_anchored(register const pcre_uchar *code, unsigned int bracket_map,
  unsigned int backref_map)
{
do {
   const pcre_uchar *scode = first_significant_code(
     code + PRIV(OP_lengths)[*code], FALSE);
   register int op = *scode;

   /* Non-capturing brackets */

   if (op == OP_BRA  || op == OP_BRAPOS ||
       op == OP_SBRA || op == OP_SBRAPOS)
     {
     if (!is_anchored(scode, bracket_map, backref_map)) return FALSE;
     }

   /* Capturing brackets */

   else if (op == OP_CBRA  || op == OP_CBRAPOS ||
            op == OP_SCBRA || op == OP_SCBRAPOS)
     {
     int n = GET2(scode, 1+LINK_SIZE);
     int new_map = bracket_map | ((n < 32)? (1 << n) : 1);
     if (!is_anchored(scode, new_map, backref_map)) return FALSE;
     }

   /* Other brackets */

   else if (op == OP_ASSERT || op == OP_ONCE || op == OP_ONCE_NC ||
            op == OP_COND)
     {
     if (!is_anchored(scode, bracket_map, backref_map)) return FALSE;
     }

   /* .* is not anchored unless DOTALL is set (which generates OP_ALLANY) and
   it isn't in brackets that are or may be referenced. */

   else if ((op == OP_TYPESTAR || op == OP_TYPEMINSTAR ||
             op == OP_TYPEPOSSTAR))
     {
     if (scode[1] != OP_ALLANY || (bracket_map & backref_map) != 0)
       return FALSE;
     }

   /* Check for explicit anchoring */

   else if (op != OP_SOD && op != OP_SOM && op != OP_CIRC) return FALSE;
   code += GET(code, 1);
   }
while (*code == OP_ALT);   /* Loop for each alternative */
return TRUE;
}



/*************************************************
*         Check for starting with ^ or .*        *
*************************************************/

/* This is called to find out if every branch starts with ^ or .* so that
"first char" processing can be done to speed things up in multiline
matching and for non-DOTALL patterns that start with .* (which must start at
the beginning or after \n). As in the case of is_anchored() (see above), we
have to take account of back references to capturing brackets that contain .*
because in that case we can't make the assumption.

Arguments:
  code           points to start of expression (the bracket)
  bracket_map    a bitmap of which brackets we are inside while testing; this
                  handles up to substring 31; after that we just have to take
                  the less precise approach
  backref_map    the back reference bitmap

Returns:         TRUE or FALSE
*/

static BOOL
is_startline(const pcre_uchar *code, unsigned int bracket_map,
  unsigned int backref_map)
{
do {
   const pcre_uchar *scode = first_significant_code(
     code + PRIV(OP_lengths)[*code], FALSE);
   register int op = *scode;

   /* If we are at the start of a conditional assertion group, *both* the
   conditional assertion *and* what follows the condition must satisfy the test
   for start of line. Other kinds of condition fail. Note that there may be an
   auto-callout at the start of a condition. */

   if (op == OP_COND)
     {
     scode += 1 + LINK_SIZE;
     if (*scode == OP_CALLOUT) scode += PRIV(OP_lengths)[OP_CALLOUT];
     switch (*scode)
       {
       case OP_CREF:
       case OP_NCREF:
       case OP_RREF:
       case OP_NRREF:
       case OP_DEF:
       return FALSE;

       default:     /* Assertion */
       if (!is_startline(scode, bracket_map, backref_map)) return FALSE;
       do scode += GET(scode, 1); while (*scode == OP_ALT);
       scode += 1 + LINK_SIZE;
       break;
       }
     scode = first_significant_code(scode, FALSE);
     op = *scode;
     }

   /* Non-capturing brackets */

   if (op == OP_BRA  || op == OP_BRAPOS ||
       op == OP_SBRA || op == OP_SBRAPOS)
     {
     if (!is_startline(scode, bracket_map, backref_map)) return FALSE;
     }

   /* Capturing brackets */

   else if (op == OP_CBRA  || op == OP_CBRAPOS ||
            op == OP_SCBRA || op == OP_SCBRAPOS)
     {
     int n = GET2(scode, 1+LINK_SIZE);
     int new_map = bracket_map | ((n < 32)? (1 << n) : 1);
     if (!is_startline(scode, new_map, backref_map)) return FALSE;
     }

   /* Other brackets */

   else if (op == OP_ASSERT || op == OP_ONCE || op == OP_ONCE_NC)
     {
     if (!is_startline(scode, bracket_map, backref_map)) return FALSE;
     }

   /* .* means "start at start or after \n" if it isn't in brackets that
   may be referenced. */

   else if (op == OP_TYPESTAR || op == OP_TYPEMINSTAR || op == OP_TYPEPOSSTAR)
     {
     if (scode[1] != OP_ANY || (bracket_map & backref_map) != 0) return FALSE;
     }

   /* Check for explicit circumflex */

   else if (op != OP_CIRC && op != OP_CIRCM) return FALSE;

   /* Move on to the next alternative */

   code += GET(code, 1);
   }
while (*code == OP_ALT);  /* Loop for each alternative */
return TRUE;
}



/*************************************************
*       Check for asserted fixed first char      *
*************************************************/

/* During compilation, the "first char" settings from forward assertions are
discarded, because they can cause conflicts with actual literals that follow.
However, if we end up without a first char setting for an unanchored pattern,
it is worth scanning the regex to see if there is an initial asserted first
char. If all branches start with the same asserted char, or with a bracket all
of whose alternatives start with the same asserted char (recurse ad lib), then
we return that char, otherwise -1.

Arguments:
  code       points to start of expression (the bracket)
  inassert   TRUE if in an assertion

Returns:     -1 or the fixed first char
*/

static int
find_firstassertedchar(const pcre_uchar *code, BOOL inassert)
{
register int c = -1;
do {
   int d;
   int xl = (*code == OP_CBRA || *code == OP_SCBRA ||
             *code == OP_CBRAPOS || *code == OP_SCBRAPOS)? IMM2_SIZE:0;
   const pcre_uchar *scode = first_significant_code(code + 1+LINK_SIZE + xl,
     TRUE);
   register int op = *scode;

   switch(op)
     {
     default:
     return -1;

     case OP_BRA:
     case OP_BRAPOS:
     case OP_CBRA:
     case OP_SCBRA:
     case OP_CBRAPOS:
     case OP_SCBRAPOS:
     case OP_ASSERT:
     case OP_ONCE:
     case OP_ONCE_NC:
     case OP_COND:
     if ((d = find_firstassertedchar(scode, op == OP_ASSERT)) < 0)
       return -1;
     if (c < 0) c = d; else if (c != d) return -1;
     break;

     case OP_EXACT:
     scode += IMM2_SIZE;
     /* Fall through */

     case OP_CHAR:
     case OP_PLUS:
     case OP_MINPLUS:
     case OP_POSPLUS:
     if (!inassert) return -1;
     if (c < 0) c = scode[1];
       else if (c != scode[1]) return -1;
     break;

     case OP_EXACTI:
     scode += IMM2_SIZE;
     /* Fall through */

     case OP_CHARI:
     case OP_PLUSI:
     case OP_MINPLUSI:
     case OP_POSPLUSI:
     if (!inassert) return -1;
     if (c < 0) c = scode[1] | REQ_CASELESS;
       else if (c != scode[1]) return -1;
     break;
     }

   code += GET(code, 1);
   }
while (*code == OP_ALT);
return c;
}



/*************************************************
*        Compile a Regular Expression            *
*************************************************/

/* This function takes a string and returns a pointer to a block of store
holding a compiled version of the expression. The original API for this
function had no error code return variable; it is retained for backwards
compatibility. The new function is given a new name.

Arguments:
  pattern       the regular expression
  options       various option bits
  errorcodeptr  pointer to error code variable (pcre_compile2() only)
                  can be NULL if you don't want a code value
  errorptr      pointer to pointer to error text
  erroroffset   ptr offset in pattern where error was detected
  tables        pointer to character tables or NULL

Returns:        pointer to compiled data block, or NULL on error,
                with errorptr and erroroffset set
*/

#ifdef COMPILE_PCRE8
PCRE_EXP_DEFN pcre * PCRE_CALL_CONVENTION
pcre_compile(const char *pattern, int options, const char **errorptr,
  int *erroroffset, const unsigned char *tables)
#else
PCRE_EXP_DEFN pcre16 * PCRE_CALL_CONVENTION
pcre16_compile(PCRE_SPTR16 pattern, int options, const char **errorptr,
  int *erroroffset, const unsigned char *tables)
#endif
{
#ifdef COMPILE_PCRE8
return pcre_compile2(pattern, options, NULL, errorptr, erroroffset, tables);
#else
return pcre16_compile2(pattern, options, NULL, errorptr, erroroffset, tables);
#endif
}


#ifdef COMPILE_PCRE8
PCRE_EXP_DEFN pcre * PCRE_CALL_CONVENTION
pcre_compile2(const char *pattern, int options, int *errorcodeptr,
  const char **errorptr, int *erroroffset, const unsigned char *tables)
#else
PCRE_EXP_DEFN pcre16 * PCRE_CALL_CONVENTION
pcre16_compile2(PCRE_SPTR16 pattern, int options, int *errorcodeptr,
  const char **errorptr, int *erroroffset, const unsigned char *tables)
#endif
{
REAL_PCRE *re;
int length = 1;  /* For final END opcode */
pcre_int32 firstchar, reqchar;
int newline;
int errorcode = 0;
int skipatstart = 0;
BOOL utf;
size_t size;
pcre_uchar *code;
const pcre_uchar *codestart;
const pcre_uchar *ptr;
compile_data compile_block;
compile_data *cd = &compile_block;

/* This space is used for "compiling" into during the first phase, when we are
computing the amount of memory that is needed. Compiled items are thrown away
as soon as possible, so that a fairly large buffer should be sufficient for
this purpose. The same space is used in the second phase for remembering where
to fill in forward references to subpatterns. That may overflow, in which case
new memory is obtained from malloc(). */

pcre_uchar cworkspace[COMPILE_WORK_SIZE];

/* Set this early so that early errors get offset 0. */

ptr = (const pcre_uchar *)pattern;

/* We can't pass back an error message if errorptr is NULL; I guess the best we
can do is just return NULL, but we can set a code value if there is a code
pointer. */

if (errorptr == NULL)
  {
  if (errorcodeptr != NULL) *errorcodeptr = 99;
  return NULL;
  }

*errorptr = NULL;
if (errorcodeptr != NULL) *errorcodeptr = ERR0;

/* However, we can give a message for this error */

if (erroroffset == NULL)
  {
  errorcode = ERR16;
  goto PCRE_EARLY_ERROR_RETURN2;
  }

*erroroffset = 0;

/* Set up pointers to the individual character tables */

if (tables == NULL) tables = PRIV(default_tables);
cd->lcc = tables + lcc_offset;
cd->fcc = tables + fcc_offset;
cd->cbits = tables + cbits_offset;
cd->ctypes = tables + ctypes_offset;

/* Check that all undefined public option bits are zero */

if ((options & ~PUBLIC_COMPILE_OPTIONS) != 0)
  {
  errorcode = ERR17;
  goto PCRE_EARLY_ERROR_RETURN;
  }

/* Check for global one-time settings at the start of the pattern, and remember
the offset for later. */

while (ptr[skipatstart] == CHAR_LEFT_PARENTHESIS &&
       ptr[skipatstart+1] == CHAR_ASTERISK)
  {
  int newnl = 0;
  int newbsr = 0;

#ifdef COMPILE_PCRE8
  if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_UTF_RIGHTPAR, 5) == 0)
    { skipatstart += 7; options |= PCRE_UTF8; continue; }
#endif
#ifdef COMPILE_PCRE16
  if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_UTF_RIGHTPAR, 6) == 0)
    { skipatstart += 8; options |= PCRE_UTF16; continue; }
#endif
  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_UCP_RIGHTPAR, 4) == 0)
    { skipatstart += 6; options |= PCRE_UCP; continue; }
  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_NO_START_OPT_RIGHTPAR, 13) == 0)
    { skipatstart += 15; options |= PCRE_NO_START_OPTIMIZE; continue; }

  if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_CR_RIGHTPAR, 3) == 0)
    { skipatstart += 5; newnl = PCRE_NEWLINE_CR; }
  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_LF_RIGHTPAR, 3)  == 0)
    { skipatstart += 5; newnl = PCRE_NEWLINE_LF; }
  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_CRLF_RIGHTPAR, 5)  == 0)
    { skipatstart += 7; newnl = PCRE_NEWLINE_CR + PCRE_NEWLINE_LF; }
  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_ANY_RIGHTPAR, 4) == 0)
    { skipatstart += 6; newnl = PCRE_NEWLINE_ANY; }
  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_ANYCRLF_RIGHTPAR, 8) == 0)
    { skipatstart += 10; newnl = PCRE_NEWLINE_ANYCRLF; }

  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_BSR_ANYCRLF_RIGHTPAR, 12) == 0)
    { skipatstart += 14; newbsr = PCRE_BSR_ANYCRLF; }
  else if (STRNCMP_UC_C8(ptr+skipatstart+2, STRING_BSR_UNICODE_RIGHTPAR, 12) == 0)
    { skipatstart += 14; newbsr = PCRE_BSR_UNICODE; }

  if (newnl != 0)
    options = (options & ~PCRE_NEWLINE_BITS) | newnl;
  else if (newbsr != 0)
    options = (options & ~(PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)) | newbsr;
  else break;
  }

/* PCRE_UTF16 has the same value as PCRE_UTF8. */
utf = UTF_ENABLED((options & PCRE_UTF8) != 0);

/* Can't support UTF unless PCRE has been compiled to include the code. The
return of an error code from PRIV(valid_utf)() is a new feature, introduced in
release 8.13. It is passed back from pcre_[dfa_]exec(), but at the moment is
not used here. */

#ifdef SUPPORT_UTF
if (utf && (options & PCRE_NO_UTF8_CHECK) == 0 &&
     (errorcode = PRIV(valid_utf)((PCRE_PUCHAR)pattern, -1, erroroffset)) != 0)
  {
#ifdef COMPILE_PCRE8
  errorcode = ERR44;
#else
  errorcode = ERR74;
#endif
  goto PCRE_EARLY_ERROR_RETURN2;
  }
#else
if (utf)
  {
  errorcode = ERR32;
  goto PCRE_EARLY_ERROR_RETURN;
  }
#endif

/* Can't support UCP unless PCRE has been compiled to include the code. */

#ifndef SUPPORT_UCP
if ((options & PCRE_UCP) != 0)
  {
  errorcode = ERR67;
  goto PCRE_EARLY_ERROR_RETURN;
  }
#endif

/* Check validity of \R options. */

if ((options & (PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE)) ==
     (PCRE_BSR_ANYCRLF|PCRE_BSR_UNICODE))
  {
  errorcode = ERR56;
  goto PCRE_EARLY_ERROR_RETURN;
  }

/* Handle different types of newline. The three bits give seven cases. The
current code allows for fixed one- or two-byte sequences, plus "any" and
"anycrlf". */

switch (options & PCRE_NEWLINE_BITS)
  {
  case 0: newline = NEWLINE; break;   /* Build-time default */
  case PCRE_NEWLINE_CR: newline = CHAR_CR; break;
  case PCRE_NEWLINE_LF: newline = CHAR_NL; break;
  case PCRE_NEWLINE_CR+
       PCRE_NEWLINE_LF: newline = (CHAR_CR << 8) | CHAR_NL; break;
  case PCRE_NEWLINE_ANY: newline = -1; break;
  case PCRE_NEWLINE_ANYCRLF: newline = -2; break;
  default: errorcode = ERR56; goto PCRE_EARLY_ERROR_RETURN;
  }

if (newline == -2)
  {
  cd->nltype = NLTYPE_ANYCRLF;
  }
else if (newline < 0)
  {
  cd->nltype = NLTYPE_ANY;
  }
else
  {
  cd->nltype = NLTYPE_FIXED;
  if (newline > 255)
    {
    cd->nllen = 2;
    cd->nl[0] = (newline >> 8) & 255;
    cd->nl[1] = newline & 255;
    }
  else
    {
    cd->nllen = 1;
    cd->nl[0] = newline;
    }
  }

/* Maximum back reference and backref bitmap. The bitmap records up to 31 back
references to help in deciding whether (.*) can be treated as anchored or not.
*/

cd->top_backref = 0;
cd->backref_map = 0;

/* Reflect pattern for debugging output */

DPRINTF(("------------------------------------------------------------------\n"));
#ifdef PCRE_DEBUG
print_puchar(stdout, (PCRE_PUCHAR)pattern);
#endif
DPRINTF(("\n"));

/* Pretend to compile the pattern while actually just accumulating the length
of memory required. This behaviour is triggered by passing a non-NULL final
argument to compile_regex(). We pass a block of workspace (cworkspace) for it
to compile parts of the pattern into; the compiled code is discarded when it is
no longer needed, so hopefully this workspace will never overflow, though there
is a test for its doing so. */

cd->bracount = cd->final_bracount = 0;
cd->names_found = 0;
cd->name_entry_size = 0;
cd->name_table = NULL;
cd->start_code = cworkspace;
cd->hwm = cworkspace;
cd->start_workspace = cworkspace;
cd->workspace_size = COMPILE_WORK_SIZE;
cd->start_pattern = (const pcre_uchar *)pattern;
cd->end_pattern = (const pcre_uchar *)(pattern + STRLEN_UC((const pcre_uchar *)pattern));
cd->req_varyopt = 0;
cd->assert_depth = 0;
cd->external_options = options;
cd->external_flags = 0;
cd->open_caps = NULL;

/* Now do the pre-compile. On error, errorcode will be set non-zero, so we
don't need to look at the result of the function here. The initial options have
been put into the cd block so that they can be changed if an option setting is
found within the regex right at the beginning. Bringing initial option settings
outside can help speed up starting point checks. */

ptr += skipatstart;
code = cworkspace;
*code = OP_BRA;
(void)compile_regex(cd->external_options, &code, &ptr, &errorcode, FALSE,
  FALSE, 0, 0, &firstchar, &reqchar, NULL, cd, &length);
if (errorcode != 0) goto PCRE_EARLY_ERROR_RETURN;

DPRINTF(("end pre-compile: length=%d workspace=%d\n", length,
  (int)(cd->hwm - cworkspace)));

if (length > MAX_PATTERN_SIZE)
  {
  errorcode = ERR20;
  goto PCRE_EARLY_ERROR_RETURN;
  }

/* Compute the size of data block needed and get it, either from malloc or
externally provided function. Integer overflow should no longer be possible
because nowadays we limit the maximum value of cd->names_found and
cd->name_entry_size. */

size = sizeof(REAL_PCRE) + (length + cd->names_found * cd->name_entry_size) * sizeof(pcre_uchar);
re = (REAL_PCRE *)(PUBL(malloc))(size);

if (re == NULL)
  {
  errorcode = ERR21;
  goto PCRE_EARLY_ERROR_RETURN;
  }

/* Put in the magic number, and save the sizes, initial options, internal
flags, and character table pointer. NULL is used for the default character
tables. The nullpad field is at the end; it's there to help in the case when a
regex compiled on a system with 4-byte pointers is run on another with 8-byte
pointers. */

re->magic_number = MAGIC_NUMBER;
re->size = (pcre_uint32)size;
re->options = cd->external_options;
re->flags = cd->external_flags;
re->dummy1 = 0;
re->first_char = 0;
re->req_char = 0;
re->name_table_offset = sizeof(REAL_PCRE) / sizeof(pcre_uchar);
re->name_entry_size = cd->name_entry_size;
re->name_count = cd->names_found;
re->ref_count = 0;
re->tables = (tables == PRIV(default_tables))? NULL : tables;
re->nullpad = NULL;

/* The starting points of the name/number translation table and of the code are
passed around in the compile data block. The start/end pattern and initial
options are already set from the pre-compile phase, as is the name_entry_size
field. Reset the bracket count and the names_found field. Also reset the hwm
field; this time it's used for remembering forward references to subpatterns.
*/

cd->final_bracount = cd->bracount;  /* Save for checking forward references */
cd->assert_depth = 0;
cd->bracount = 0;
cd->names_found = 0;
cd->name_table = (pcre_uchar *)re + re->name_table_offset;
codestart = cd->name_table + re->name_entry_size * re->name_count;
cd->start_code = codestart;
cd->hwm = (pcre_uchar *)(cd->start_workspace);
cd->req_varyopt = 0;
cd->had_accept = FALSE;
cd->check_lookbehind = FALSE;
cd->open_caps = NULL;

/* Set up a starting, non-extracting bracket, then compile the expression. On
error, errorcode will be set non-zero, so we don't need to look at the result
of the function here. */

ptr = (const pcre_uchar *)pattern + skipatstart;
code = (pcre_uchar *)codestart;
*code = OP_BRA;
(void)compile_regex(re->options, &code, &ptr, &errorcode, FALSE, FALSE, 0, 0,
  &firstchar, &reqchar, NULL, cd, NULL);
re->top_bracket = cd->bracount;
re->top_backref = cd->top_backref;
re->flags = cd->external_flags | PCRE_MODE;

if (cd->had_accept) reqchar = REQ_NONE;   /* Must disable after (*ACCEPT) */

/* If not reached end of pattern on success, there's an excess bracket. */

if (errorcode == 0 && *ptr != 0) errorcode = ERR22;

/* Fill in the terminating state and check for disastrous overflow, but
if debugging, leave the test till after things are printed out. */

*code++ = OP_END;

#ifndef PCRE_DEBUG
if (code - codestart > length) errorcode = ERR23;
#endif

/* Fill in any forward references that are required. There may be repeated
references; optimize for them, as searching a large regex takes time. */

if (cd->hwm > cd->start_workspace)
  {
  int prev_recno = -1;
  const pcre_uchar *groupptr = NULL;
  while (errorcode == 0 && cd->hwm > cd->start_workspace)
    {
    int offset, recno;
    cd->hwm -= LINK_SIZE;
    offset = GET(cd->hwm, 0);
    recno = GET(codestart, offset);
    if (recno != prev_recno)
      {
      groupptr = PRIV(find_bracket)(codestart, utf, recno);
      prev_recno = recno;
      }
    if (groupptr == NULL) errorcode = ERR53;
      else PUT(((pcre_uchar *)codestart), offset, (int)(groupptr - codestart));
    }
  }

/* If the workspace had to be expanded, free the new memory. */

if (cd->workspace_size > COMPILE_WORK_SIZE)
  (PUBL(free))((void *)cd->start_workspace);

/* Give an error if there's back reference to a non-existent capturing
subpattern. */

if (errorcode == 0 && re->top_backref > re->top_bracket) errorcode = ERR15;

/* If there were any lookbehind assertions that contained OP_RECURSE
(recursions or subroutine calls), a flag is set for them to be checked here,
because they may contain forward references. Actual recursions can't be fixed
length, but subroutine calls can. It is done like this so that those without
OP_RECURSE that are not fixed length get a diagnosic with a useful offset. The
exceptional ones forgo this. We scan the pattern to check that they are fixed
length, and set their lengths. */

if (cd->check_lookbehind)
  {
  pcre_uchar *cc = (pcre_uchar *)codestart;

  /* Loop, searching for OP_REVERSE items, and process those that do not have
  their length set. (Actually, it will also re-process any that have a length
  of zero, but that is a pathological case, and it does no harm.) When we find
  one, we temporarily terminate the branch it is in while we scan it. */

  for (cc = (pcre_uchar *)PRIV(find_bracket)(codestart, utf, -1);
       cc != NULL;
       cc = (pcre_uchar *)PRIV(find_bracket)(cc, utf, -1))
    {
    if (GET(cc, 1) == 0)
      {
      int fixed_length;
      pcre_uchar *be = cc - 1 - LINK_SIZE + GET(cc, -LINK_SIZE);
      int end_op = *be;
      *be = OP_END;
      fixed_length = find_fixedlength(cc, (re->options & PCRE_UTF8) != 0, TRUE,
        cd);
      *be = end_op;
      DPRINTF(("fixed length = %d\n", fixed_length));
      if (fixed_length < 0)
        {
        errorcode = (fixed_length == -2)? ERR36 :
                    (fixed_length == -4)? ERR70 : ERR25;
        break;
        }
      PUT(cc, 1, fixed_length);
      }
    cc += 1 + LINK_SIZE;
    }
  }

/* Failed to compile, or error while post-processing */

if (errorcode != 0)
  {
  (PUBL(free))(re);
  PCRE_EARLY_ERROR_RETURN:
  *erroroffset = (int)(ptr - (const pcre_uchar *)pattern);
  PCRE_EARLY_ERROR_RETURN2:
  *errorptr = find_error_text(errorcode);
  if (errorcodeptr != NULL) *errorcodeptr = errorcode;
  return NULL;
  }

/* If the anchored option was not passed, set the flag if we can determine that
the pattern is anchored by virtue of ^ characters or \A or anything else (such
as starting with .* when DOTALL is set).

Otherwise, if we know what the first byte has to be, save it, because that
speeds up unanchored matches no end. If not, see if we can set the
PCRE_STARTLINE flag. This is helpful for multiline matches when all branches
start with ^. and also when all branches start with .* for non-DOTALL matches.
*/

if ((re->options & PCRE_ANCHORED) == 0)
  {
  if (is_anchored(codestart, 0, cd->backref_map))
    re->options |= PCRE_ANCHORED;
  else
    {
    if (firstchar < 0)
      firstchar = find_firstassertedchar(codestart, FALSE);
    if (firstchar >= 0)   /* Remove caseless flag for non-caseable chars */
      {
#ifdef COMPILE_PCRE8
      re->first_char = firstchar & 0xff;
#else
#ifdef COMPILE_PCRE16
      re->first_char = firstchar & 0xffff;
#endif
#endif
      if ((firstchar & REQ_CASELESS) != 0)
        {
#if defined SUPPORT_UCP && !(defined COMPILE_PCRE8)
        /* We ignore non-ASCII first chars in 8 bit mode. */
        if (utf)
          {
          if (re->first_char < 128)
            {
            if (cd->fcc[re->first_char] != re->first_char)
              re->flags |= PCRE_FCH_CASELESS;
            }
          else if (UCD_OTHERCASE(re->first_char) != re->first_char)
            re->flags |= PCRE_FCH_CASELESS;
          }
        else
#endif
        if (MAX_255(re->first_char)
            && cd->fcc[re->first_char] != re->first_char)
          re->flags |= PCRE_FCH_CASELESS;
        }

      re->flags |= PCRE_FIRSTSET;
      }
    else if (is_startline(codestart, 0, cd->backref_map))
      re->flags |= PCRE_STARTLINE;
    }
  }

/* For an anchored pattern, we use the "required byte" only if it follows a
variable length item in the regex. Remove the caseless flag for non-caseable
bytes. */

if (reqchar >= 0 &&
     ((re->options & PCRE_ANCHORED) == 0 || (reqchar & REQ_VARY) != 0))
  {
#ifdef COMPILE_PCRE8
  re->req_char = reqchar & 0xff;
#else
#ifdef COMPILE_PCRE16
  re->req_char = reqchar & 0xffff;
#endif
#endif
  if ((reqchar & REQ_CASELESS) != 0)
    {
#if defined SUPPORT_UCP && !(defined COMPILE_PCRE8)
    /* We ignore non-ASCII first chars in 8 bit mode. */
    if (utf)
      {
      if (re->req_char < 128)
        {
        if (cd->fcc[re->req_char] != re->req_char)
          re->flags |= PCRE_RCH_CASELESS;
        }
      else if (UCD_OTHERCASE(re->req_char) != re->req_char)
        re->flags |= PCRE_RCH_CASELESS;
      }
    else
#endif
    if (MAX_255(re->req_char) && cd->fcc[re->req_char] != re->req_char)
      re->flags |= PCRE_RCH_CASELESS;
    }

  re->flags |= PCRE_REQCHSET;
  }

/* Print out the compiled data if debugging is enabled. This is never the
case when building a production library. */

#ifdef PCRE_DEBUG
printf("Length = %d top_bracket = %d top_backref = %d\n",
  length, re->top_bracket, re->top_backref);

printf("Options=%08x\n", re->options);

if ((re->flags & PCRE_FIRSTSET) != 0)
  {
  pcre_uchar ch = re->first_char;
  const char *caseless =
    ((re->flags & PCRE_FCH_CASELESS) == 0)? "" : " (caseless)";
  if (PRINTABLE(ch)) printf("First char = %c%s\n", ch, caseless);
    else printf("First char = \\x%02x%s\n", ch, caseless);
  }

if ((re->flags & PCRE_REQCHSET) != 0)
  {
  pcre_uchar ch = re->req_char;
  const char *caseless =
    ((re->flags & PCRE_RCH_CASELESS) == 0)? "" : " (caseless)";
  if (PRINTABLE(ch)) printf("Req char = %c%s\n", ch, caseless);
    else printf("Req char = \\x%02x%s\n", ch, caseless);
  }

#ifdef COMPILE_PCRE8
pcre_printint((pcre *)re, stdout, TRUE);
#else
pcre16_printint((pcre *)re, stdout, TRUE);
#endif

/* This check is done here in the debugging case so that the code that
was compiled can be seen. */

if (code - codestart > length)
  {
  (PUBL(free))(re);
  *errorptr = find_error_text(ERR23);
  *erroroffset = ptr - (pcre_uchar *)pattern;
  if (errorcodeptr != NULL) *errorcodeptr = ERR23;
  return NULL;
  }
#endif   /* PCRE_DEBUG */

#ifdef COMPILE_PCRE8
return (pcre *)re;
#else
return (pcre16 *)re;
#endif
}

/* End of pcre_compile.c */
