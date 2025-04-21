/*
 * pthread.c
 *
 * Description:
 * This translation unit agregates pthreads-win32 translation units.
 * It is used for inline optimisation of the library,
 * maximising for speed at the expense of size.
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

/* The following are ordered for inlining */

#include "pthread_ptw32_MCS_lock.c.h"
#include "pthread_ptw32_is_attr.c.h"
#include "pthread_ptw32_processInitialize.c.h"
#include "pthread_ptw32_processTerminate.c.h"
#include "pthread_ptw32_threadStart.c.h"
#include "pthread_ptw32_threadDestroy.c.h"
#include "pthread_ptw32_tkAssocCreate.c.h"
#include "pthread_ptw32_tkAssocDestroy.c.h"
#include "pthread_ptw32_callUserDestroyRoutines.c.h"
#include "pthread_ptw32_semwait.c.h"
#include "pthread_ptw32_timespec.c.h"
#include "pthread_ptw32_throw.c.h"
#include "pthread_ptw32_getprocessors.c.h"
#include "pthread_ptw32_calloc.c.h"
#include "pthread_ptw32_new.c.h"
#include "pthread_ptw32_reuse.c.h"
#include "pthread_ptw32_relmillisecs.c.h"
#include "pthread_ptw32_cond_check_need_init.c.h"
#include "pthread_ptw32_mutex_check_need_init.c.h"
#include "pthread_ptw32_rwlock_check_need_init.c.h"
#include "pthread_ptw32_rwlock_cancelwrwait.c.h"
#include "pthread_ptw32_spinlock_check_need_init.c.h"
#include "pthread_attr_init.c.h"
#include "pthread_attr_destroy.c.h"
#include "pthread_attr_getdetachstate.c.h"
#include "pthread_attr_setdetachstate.c.h"
#include "pthread_attr_getscope.c.h"
#include "pthread_attr_setscope.c.h"
#include "pthread_attr_getstackaddr.c.h"
#include "pthread_attr_setstackaddr.c.h"
#include "pthread_attr_getstacksize.c.h"
#include "pthread_attr_setstacksize.c.h"
#include "pthread_barrier_init.c.h"
#include "pthread_barrier_destroy.c.h"
#include "pthread_barrier_wait.c.h"
#include "pthread_barrierattr_init.c.h"
#include "pthread_barrierattr_destroy.c.h"
#include "pthread_barrierattr_setpshared.c.h"
#include "pthread_barrierattr_getpshared.c.h"
#include "pthread_setcancelstate.c.h"
#include "pthread_setcanceltype.c.h"
#include "pthread_testcancel.c.h"
#include "pthread_cancel2.c.h"
#include "pthread_condattr_destroy.c.h"
#include "pthread_condattr_getpshared.c.h"
#include "pthread_condattr_init.c.h"
#include "pthread_condattr_setpshared.c.h"
#include "pthread_cond_destroy.c.h"
#include "pthread_cond_init.c.h"
#include "pthread_cond_signal.c.h"
#include "pthread_cond_wait.c.h"
#include "pthread_create.c.h"
#include "pthread_cleanup.c.h"
#include "pthread_dll.c.h"
#include "pthread_autostatic.c.h"
#include "pthread_errno.c.h"
#include "pthread_exit2.c.h"
#include "pthread_global.c.h"
#include "pthread_equal.c.h"
#include "pthread_getconcurrency.c.h"
#include "pthread_kill.c.h"
#include "pthread_once.c.h"
#include "pthread_self.c.h"
#include "pthread_setconcurrency.c.h"
#include "pthread_w32_CancelableWait.c.h"
#include "pthread_mutex_init.c.h"
#include "pthread_mutex_destroy.c.h"
#include "pthread_mutexattr_init.c.h"
#include "pthread_mutexattr_destroy.c.h"
#include "pthread_mutexattr_getpshared.c.h"
#include "pthread_mutexattr_setpshared.c.h"
#include "pthread_mutexattr_settype.c.h"
#include "pthread_mutexattr_gettype.c.h"
#include "pthread_mutexattr_setrobust.c.h"
#include "pthread_mutexattr_getrobust.c.h"
#include "pthread_mutex_lock.c.h"
#include "pthread_mutex_timedlock.c.h"
#include "pthread_mutex_unlock.c.h"
#include "pthread_mutex_trylock.c.h"
#include "pthread_mutex_consistent.c.h"
#include "pthread_mutexattr_setkind_np.c.h"
#include "pthread_mutexattr_getkind_np.c.h"
#include "pthread_getw32threadhandle_np.c.h"
#include "pthread_getunique_np.c.h"
#include "pthread_timedjoin_np.c.h"
#include "pthread_tryjoin_np.c.h"
#include "pthread_setaffinity.c.h"
#include "pthread_delay_np.c.h"
#include "pthread_num_processors_np.c.h"
#include "pthread_win32_attach_detach_np.c.h"
#include "pthread_timechange_handler_np.c.h"
#include "pthread_rwlock_init.c.h"
#include "pthread_rwlock_destroy.c.h"
#include "pthread_rwlockattr_init.c.h"
#include "pthread_rwlockattr_destroy.c.h"
#include "pthread_rwlockattr_getpshared.c.h"
#include "pthread_rwlockattr_setpshared.c.h"
#include "pthread_rwlock_rdlock.c.h"
#include "pthread_rwlock_timedrdlock.c.h"
#include "pthread_rwlock_wrlock.c.h"
#include "pthread_rwlock_timedwrlock.c.h"
#include "pthread_rwlock_unlock.c.h"
#include "pthread_rwlock_tryrdlock.c.h"
#include "pthread_rwlock_trywrlock.c.h"
#include "pthread_attr_setschedpolicy.c.h"
#include "pthread_attr_getschedpolicy.c.h"
#include "pthread_attr_setschedparam.c.h"
#include "pthread_attr_getschedparam.c.h"
#include "pthread_attr_setinheritsched.c.h"
#include "pthread_attr_getinheritsched.c.h"
#include "pthread_setschedparam.c.h"
#include "pthread_getschedparam.c.h"
#include "pthread_sched_get_priority_max.c.h"
#include "pthread_sched_get_priority_min.c.h"
#include "pthread_sched_setscheduler.c.h"
#include "pthread_sched_getscheduler.c.h"
#include "pthread_sched_yield.c.h"
#include "pthread_sched_setaffinity.c.h"
#include "pthread_sem_init.c.h"
#include "pthread_sem_destroy.c.h"
#include "pthread_sem_trywait.c.h"
#include "pthread_sem_timedwait.c.h"
#include "pthread_sem_wait.c.h"
#include "pthread_sem_post.c.h"
#include "pthread_sem_post_multiple.c.h"
#include "pthread_sem_getvalue.c.h"
#include "pthread_sem_open.c.h"
#include "pthread_sem_close.c.h"
#include "pthread_sem_unlink.c.h"
#include "pthread_spin_init.c.h"
#include "pthread_spin_destroy.c.h"
#include "pthread_spin_lock.c.h"
#include "pthread_spin_unlock.c.h"
#include "pthread_spin_trylock.c.h"
#include "pthread_detach.c.h"
#include "pthread_join.c.h"
#include "pthread_key_create.c.h"
#include "pthread_key_delete.c.h"
#include "pthread_setspecific.c.h"
#include "pthread_getspecific.c.h"
