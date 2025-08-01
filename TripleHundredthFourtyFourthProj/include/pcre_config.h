﻿/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

//#include "pcre_config.h" // Allow AutoHotkey config.h to define charset (_MBCS or UNICODE).

/* On Unix-like systems config.h.in is converted by "configure" into config.h.
Some other environments also support the use of "configure". PCRE is written in
Standard C, but there are a few non-standard things it can cope with, allowing
it to run on SunOS4 and other "close to standard" systems.

If you are going to build PCRE "by hand" on a system without "configure" you
should copy the distributed config.h.generic to config.h, and then set up the
macro definitions the way you need them. You must then add -DHAVE_CONFIG_H to
all of your compile commands, so that config.h is included at the start of
every source.

Alternatively, you can avoid editing by using -D on the compiler command line
to set the macro values. In this case, you do not have to set -DHAVE_CONFIG_H.

PCRE uses memmove() if HAVE_MEMMOVE is set to 1; otherwise it uses bcopy() if
HAVE_BCOPY is set to 1. If your system has neither bcopy() nor memmove(), set
them both to 0; an emulation function will be used. */

/* By default, the \R escape sequence matches any Unicode line ending
   character or sequence of characters. If BSR_ANYCRLF is defined, this is
   changed so that backslash-R matches only CR, LF, or CRLF. The build-time
   default can be overridden by the user of PCRE at runtime. On systems that
   support it, "configure" can be used to override the default. */
/* #undef BSR_ANYCRLF */
/* AutoHotkey: BSR_ANYCRLF above is left undefined for backward compatibility with earlier versions
of AutoHotkey.  In any case, it might be the best default because it matches up with the default behavior
of `a (PCRE_NEWLINE_ANY), namely that all of the following are considered newlines:
`r, `n, `r`n, `v/VT/vertical tab/chr(0xB), `f/FF/formfeed/chr(0xC), and NEL/next-line/chr(0x85).*/

/* If you are compiling for a system that uses EBCDIC instead of ASCII
   character codes, define this macro as 1. On systems that can use
   "configure", this can be done via --enable-ebcdic. PCRE will then assume
   that all input strings are in EBCDIC. If you do not define this macro, PCRE
   will assume input strings are ASCII or UTF-8/16 Unicode. It is not possible
   to build a version of PCRE that supports both EBCDIC and UTF-8/16. */
/* #undef EBCDIC */

/* Define to 1 if you have the `bcopy' function. */
#ifndef HAVE_BCOPY
#define HAVE_BCOPY 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if you have the <bits/type_traits.h> header file. */
/* #undef HAVE_BITS_TYPE_TRAITS_H */

/* Define to 1 if you have the <bzlib.h> header file. */
/* #undef HAVE_BZLIB_H */

/* Define to 1 if you have the <dirent.h> header file. */
#ifndef HAVE_DIRENT_H
#define HAVE_DIRENT_H 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef HAVE_DLFCN_H
#define HAVE_DLFCN_H 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef HAVE_INTTYPES_H
#define HAVE_INTTYPES_H 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if you have the <limits.h> header file. */
#ifndef HAVE_LIMITS_H
#define HAVE_LIMITS_H 1
#endif

/* Define to 1 if the system has the type `long long'. */
#ifndef HAVE_LONG_LONG
#define HAVE_LONG_LONG 1
#endif

/* Define to 1 if you have the `memmove' function. */
#ifndef HAVE_MEMMOVE
#define HAVE_MEMMOVE 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef HAVE_MEMORY_H
#define HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the <readline/history.h> header file. */
/* #undef HAVE_READLINE_HISTORY_H */

/* Define to 1 if you have the <readline/readline.h> header file. */
/* #undef HAVE_READLINE_READLINE_H */

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the `strerror' function. */
#ifndef HAVE_STRERROR
#define HAVE_STRERROR 1
#endif

/* Define to 1 if you have the <string> header file. */
#ifndef HAVE_STRING
#define HAVE_STRING 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef HAVE_STRINGS_H
#define HAVE_STRINGS_H 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif

/* Define to 1 if you have `strtoimax'. */
/* #undef HAVE_STRTOIMAX */

/* Define to 1 if you have `strtoll'. */
#ifndef HAVE_STRTOLL
#define HAVE_STRTOLL 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if you have `strtoq'. */
#ifndef HAVE_STRTOQ
#define HAVE_STRTOQ 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef HAVE_SYS_TYPES_H
#define HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have the <type_traits.h> header file. */
/* #undef HAVE_TYPE_TRAITS_H */

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 0  /* AutoHotkey: Changed from 1 to 0. */
#endif

/* Define to 1 if the system has the type `unsigned long long'. */
#ifndef HAVE_UNSIGNED_LONG_LONG
#define HAVE_UNSIGNED_LONG_LONG 1
#endif

/* Define to 1 if you have the <windows.h> header file. */
/* #undef HAVE_WINDOWS_H */
#define HAVE_WINDOWS_H 1  /* AutoHotkey: Added. */

/* Define to 1 if you have the <zlib.h> header file. */
/* #undef HAVE_ZLIB_H */

/* Define to 1 if you have `_strtoi64'. */
#define HAVE__STRTOI64 1  /* AutoHotkey: Added. */

/* The value of LINK_SIZE determines the number of bytes used to store links
   as offsets within the compiled regex. The default is 2, which allows for
   compiled patterns up to 64K long. This covers the vast majority of cases.
   However, PCRE can also be compiled to use 3 or 4 bytes instead. This allows
   for longer patterns in extreme cases. On systems that support it,
   "configure" can be used to override this default. */
#ifndef LINK_SIZE
#define LINK_SIZE 2  /* AutoHotkey: PHP also seems to use 2, so it's likely the best compromise between memory utilization and being able to handle unusually large compiled patterns. */
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
/* #undef LT_OBJDIR */

/* The value of MATCH_LIMIT determines the default number of times the
   internal match() function can be called during a single execution of
   pcre_exec(). There is a runtime interface for setting a different limit.
   The limit exists in order to catch runaway regular expressions that take
   for ever to determine that they do not match. The default is set very large
   so that it does not accidentally catch legitimate cases. On systems that
   support it, "configure" can be used to override this default default. */
#ifndef MATCH_LIMIT
#define MATCH_LIMIT 10000000
#endif

/* The above limit applies to all calls of match(), whether or not they
   increase the recursion depth. In some environments it is desirable to limit
   the depth of recursive calls of match() more strictly, in order to restrict
   the maximum amount of stack (or heap, if NO_RECURSE is defined) that is
   used. The value of MATCH_LIMIT_RECURSION applies only to recursive calls of
   match(). To have any useful effect, it must be less than the value of
   MATCH_LIMIT. The default is to use the same value as MATCH_LIMIT. There is
   a runtime method for setting a different limit. On systems that support it,
   "configure" can be used to override the default. */
/* AutoHotkey: Original value was MATCH_LIMIT (i.e. 10000000).  It was lowered to 6000 so that
the program's current stack limit of 4 MB won't be exceeded. The limit was computed from the
following info in PCRE docs somewhere: "As a very rough rule of thumb, you should reckon on
about 500 bytes per recursion. Thus, if you want to limit your stack usage to 8Mb, you should
set the limit at 16000 recursions. A 64Mb stack, on the other hand, can support around
128000 recursions."
*/
#ifndef MATCH_LIMIT_RECURSION
#define MATCH_LIMIT_RECURSION 6000
#endif

/* This limit is parameterized just in case anybody ever wants to change it.
   Care must be taken if it is increased, because it guards against integer
   overflow caused by enormously large patterns. */
#ifndef MAX_NAME_COUNT
#define MAX_NAME_COUNT 10000
#endif

/* This limit is parameterized just in case anybody ever wants to change it.
   Care must be taken if it is increased, because it guards against integer
   overflow caused by enormously large patterns. */
#ifndef MAX_NAME_SIZE
#define MAX_NAME_SIZE 32
#endif

/* The value of NEWLINE determines the newline character sequence. On systems
   that support it, "configure" can be used to override the default, which is
   10. The possible values are 10 (LF), 13 (CR), 3338 (CRLF), -1 (ANY), or -2
   (ANYCRLF). */
/* AutoHotkey: Changed below from default of 10 to 3338 because there's a
   slight chance that compiling this way improves performance. */
#ifndef NEWLINE
#define NEWLINE 3338
#endif

/* PCRE uses recursive function calls to handle backtracking while matching.
   This can sometimes be a problem on systems that have stacks of limited
   size. Define NO_RECURSE to get a version that doesn't use recursion in the
   match() function; instead it creates its own stack by steam using
   pcre_recurse_malloc() to obtain memory from the heap. For more detail, see
   the comments and other stuff just above the match() function. On systems
   that support it, "configure" can be used to set this in the Makefile (use
   --disable-stack-for-recursion). */
/* #undef NO_RECURSE */

/* Name of package */
#define PACKAGE "pcre"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "PCRE"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "PCRE 8.30"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "pcre"

/* Define to the home page for this package. */
/* #undef PACKAGE_URL */

/* Define to the version of this package. */
#define PACKAGE_VERSION "8.30"

/* The value of PCREGREP_BUFSIZE determines the size of buffer used by
   pcregrep to hold parts of the file it is searching. On systems that support
   it, "configure" can be used to override the default, which is 8192. This is
   also the minimum value. The actual amount of memory used by pcregrep is
   three times this number, because it allows for the buffering of "before"
   and "after" lines. */
#ifndef PCREGREP_BUFSIZE
#define PCREGREP_BUFSIZE 20480
#endif


/* If you are compiling for a system other than a Unix-like system or
   Win32, and it needs some magic to be inserted before the definition
   of a function that is exported by the library, define this macro to
   contain the relevant magic. If you do not define this macro, it
   defaults to "extern" for a C compiler and "extern C" for a C++
   compiler on non-Win32 systems. This macro apears at the start of
   every exported function that is part of the external API. It does
   not appear on functions that are "external" in the C sense, but
   which are internal to the library. */
/* #undef PCRE_EXP_DEFN */

/* Define if linking statically (TODO: make nice with Libtool) */
/* #undef PCRE_STATIC */
#define PCRE_STATIC 1  /* AutoHotkey: Added because it reduces code size of AutoHotkey.exe and others by 1-2 KB.  It's also more appropriate since the linking of this library is at compile-time (i.e. not a DLL). */

/* When calling PCRE via the POSIX interface, additional working storage is
   required for holding the pointers to capturing substrings because PCRE
   requires three integers per substring, whereas the POSIX interface provides
   only two. If the number of expected substrings is small, the wrapper
   function uses space on the stack, because this is faster than using
   malloc() for each call. The threshold above which the stack is no longer
   used is defined by POSIX_MALLOC_THRESHOLD. On systems that support it,
   "configure" can be used to override this default. */
#ifndef POSIX_MALLOC_THRESHOLD
#define POSIX_MALLOC_THRESHOLD 10
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef STDC_HEADERS
#define STDC_HEADERS 1
#endif

/* Define to enable support for Just-In-Time compiling. */
/* #undef SUPPORT_JIT */

/* Define to allow pcregrep to be linked with libbz2, so that it is able to
   handle .bz2 files. */
/* #undef SUPPORT_LIBBZ2 */

/* Define to allow pcretest to be linked with libreadline. */
/* #undef SUPPORT_LIBREADLINE */

/* Define to allow pcregrep to be linked with libz, so that it is able to
   handle .gz files. */
/* #undef SUPPORT_LIBZ */

#ifdef UNICODE

/* Define to enable the 16 bit PCRE library. */
#define SUPPORT_PCRE16

/* Define to enable support for Unicode properties */
#define SUPPORT_UCP


/* Define to enable support for the UTF-8/16 Unicode encoding. This will work
   even in an EBCDIC environment, but it is incompatible with the EBCDIC
   macro. That is, PCRE can support *either* EBCDIC code *or* ASCII/UTF-8/16,
   but not both at once. */
#define SUPPORT_UTF

/* AutoHotkey: Define to validate UTF input. Leave undefined for performance. */
/* #undef SUPPORT_UTF_VALIDATION */

#else

/* Define to enable the 8 bit PCRE library. */
#define SUPPORT_PCRE8

#endif

/* Define to enable JIT support in pcregrep. */
/* #undef SUPPORT_PCREGREP_JIT */

/* Version number of package */
#ifndef VERSION
#define VERSION "8.30"
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to the type of a signed integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
/* #define int64_t __int64 */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
