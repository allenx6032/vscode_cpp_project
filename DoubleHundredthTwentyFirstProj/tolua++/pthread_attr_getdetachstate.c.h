/*
 * pthread_attr_getdetachstate.c
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


int
pthread_attr_getdetachstate (const pthread_attr_t * attr, int *detachstate)
     /*
      * ------------------------------------------------------
      * DOCPUBLIC
      *      This function determines whether threads created with
      *      'attr' will run detached.
      *
      * PARAMETERS
      *      attr
      *              pointer to an instance of pthread_attr_t
      *
      *      detachstate
      *              pointer to an integer into which is returned one
      *              of:
      *
      *              PTHREAD_CREATE_JOINABLE
      *                              Thread ID is valid, must be joined
      *
      *              PTHREAD_CREATE_DETACHED
      *                              Thread ID is invalid, cannot be joined,
      *                              canceled, or modified
      *
      *
      * DESCRIPTION
      *      This function determines whether threads created with
      *      'attr' will run detached.
      *
      *      NOTES:
      *              1)      You cannot join or cancel detached threads.
      *
      * RESULTS
      *              0               successfully retrieved detach state,
      *              EINVAL          'attr' is invalid
      *
      * ------------------------------------------------------
      */
{
  if (ptw32_is_attr (attr) != 0 || detachstate == NULL)
    {
      return EINVAL;
    }

  *detachstate = (*attr)->detachstate;
  return 0;
}
