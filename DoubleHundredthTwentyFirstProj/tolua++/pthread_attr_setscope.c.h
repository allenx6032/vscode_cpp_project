/*
 * pthread_attr_setscope.c
 *
 * Description:
 * This translation unit implements operations on thread attribute objects.
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2012 Pthreads-win32 contributors
 *
 *      Homepage1: http://sourceware.org/pthreads-win32/
 *      Homepage2: http://sourceforge.net/projects/pthreads4w/
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 * 
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 * 
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 * 
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "pthread.h"
#include "pthread_implement.h"

/* ignore warning "unreferenced formal parameter" */
#if defined(_MSC_VER)
#pragma warning( disable : 4100 )
#endif

int
pthread_attr_setscope (pthread_attr_t * attr, int contentionscope)
{
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
  switch (contentionscope)
    {
    case PTHREAD_SCOPE_SYSTEM:
      (*attr)->contentionscope = contentionscope;
      return 0;
    case PTHREAD_SCOPE_PROCESS:
      return ENOTSUP;
    default:
      return EINVAL;
    }
#else
  return ENOSYS;
#endif
}
