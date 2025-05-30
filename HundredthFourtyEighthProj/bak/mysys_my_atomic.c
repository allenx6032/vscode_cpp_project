/* Copyright (C) 2006 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <libmysql_my_global.h>
#include <libmysql_my_sys.h>

#ifndef HAVE_INLINE
/* the following will cause all inline functions to be instantiated */
#define HAVE_INLINE
#undef STATIC_INLINE
#define STATIC_INLINE extern
#endif

#include <libmysql_my_atomic.h>

/*
  checks that the current build of atomic ops
  can run on this machine

  RETURN
    ATOMIC_xxx values, see my_atomic.h
*/
int my_atomic_initialize()
{
  compile_time_assert(sizeof(intptr) == sizeof(void *));
  /* currently the only thing worth checking is SMP/UP issue */
#ifdef MY_ATOMIC_MODE_DUMMY
  return my_getncpus() == 1 ? MY_ATOMIC_OK : MY_ATOMIC_NOT_1CPU;
#else
  return MY_ATOMIC_OK;
#endif
}

