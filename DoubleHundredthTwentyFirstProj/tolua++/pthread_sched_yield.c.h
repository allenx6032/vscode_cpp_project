/*
 * sched_yield.c
 * 
 * Description:
 * POSIX thread functions that deal with thread scheduling.
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
#include "sched.h"

int
sched_yield (void)
     /*
      * ------------------------------------------------------
      * DOCPUBLIC
      *      This function indicates that the calling thread is
      *      willing to give up some time slices to other threads.
      *
      * PARAMETERS
      *      N/A
      *
      *
      * DESCRIPTION
      *      This function indicates that the calling thread is
      *      willing to give up some time slices to other threads.
      *      NOTE: Since this is part of POSIX 1003.1b
      *                (realtime extensions), it is defined as returning
      *                -1 if an error occurs and sets errno to the actual
      *                error.
      *
      * RESULTS
      *              0               successfully created semaphore,
      *              ENOSYS          sched_yield not supported,
      *
      * ------------------------------------------------------
      */
{
  Sleep (0);

  return 0;
}
