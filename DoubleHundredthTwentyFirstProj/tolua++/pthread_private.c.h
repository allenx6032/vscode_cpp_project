/*
 * private.c
 *
 * Description:
 * This translation unit implements routines which are private to
 * the implementation and may be used throughout it.
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 * 
 *      Contact Email: rpj@callisto.canberra.edu.au
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

#include "pthread.h"
#include "pthread_implement.h"

#include "pthread_ptw32_MCS_lock.c"
#include "pthread_ptw32_is_attr.c"
#include "pthread_ptw32_processInitialize.c"
#include "pthread_ptw32_processTerminate.c"
#include "pthread_ptw32_threadStart.c"
#include "pthread_ptw32_threadDestroy.c"
#include "pthread_ptw32_tkAssocCreate.c"
#include "pthread_ptw32_tkAssocDestroy.c"
#include "pthread_ptw32_callUserDestroyRoutines.c"
#include "pthread_ptw32_semwait.c"
#include "pthread_ptw32_timespec.c"
#include "pthread_ptw32_relmillisecs.c"
#include "pthread_ptw32_throw.c"
#include "pthread_ptw32_getprocessors.c"
#include "pthread_ptw32_new.c"
#include "pthread_ptw32_calloc.c"
#include "pthread_ptw32_reuse.c"
#include "pthread_ptw32_cond_check_need_init.c"
#include "pthread_ptw32_mutex_check_need_init.c"
#include "pthread_ptw32_rwlock_check_need_init.c"
#include "pthread_ptw32_rwlock_cancelwrwait.c"
#include "pthread_ptw32_spinlock_check_need_init.c"
