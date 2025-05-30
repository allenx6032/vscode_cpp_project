/*
 * pthread_mutex_lock.c
 *
 * Description:
 * This translation unit implements mutual exclusion (mutex) primitives.
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

#if !defined(_UWIN)
/*#   include <process.h> */
#endif
#include "pthread.h"
#include "pthread_implement.h"

int
pthread_mutex_lock (pthread_mutex_t * mutex)
{
  int kind;
  pthread_mutex_t mx;
  int result = 0;

  /*
   * Let the system deal with invalid pointers.
   */
  if (*mutex == NULL)
    {
      return EINVAL;
    }

  /*
   * We do a quick check to see if we need to do more work
   * to initialise a static mutex. We check
   * again inside the guarded section of ptw32_mutex_check_need_init()
   * to avoid race conditions.
   */
  if (*mutex >= PTHREAD_ERRORCHECK_MUTEX_INITIALIZER)
    {
      if ((result = ptw32_mutex_check_need_init (mutex)) != 0)
	{
	  return (result);
	}
    }

  mx = *mutex;
  kind = mx->kind;

  if (kind >= 0)
    {
      /* Non-robust */
      if (PTHREAD_MUTEX_NORMAL == kind)
        {
          if ((PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_EXCHANGE_LONG(
		       (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
		       (PTW32_INTERLOCKED_LONG) 1) != 0)
	    {
	      while ((PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_EXCHANGE_LONG(
                              (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
			      (PTW32_INTERLOCKED_LONG) -1) != 0)
	        {
	          if (WAIT_OBJECT_0 != WaitForSingleObject (mx->event, INFINITE))
	            {
	              result = EINVAL;
		      break;
	            }
	        }
	    }
        }
      else
        {
          pthread_t self = pthread_self();

          if ((PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG(
                       (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
		       (PTW32_INTERLOCKED_LONG) 1,
		       (PTW32_INTERLOCKED_LONG) 0) == 0)
	    {
	      mx->recursive_count = 1;
	      mx->ownerThread = self;
	    }
          else
	    {
	      if (pthread_equal (mx->ownerThread, self))
	        {
	          if (kind == PTHREAD_MUTEX_RECURSIVE)
		    {
		      mx->recursive_count++;
		    }
	          else
		    {
		      result = EDEADLK;
		    }
	        }
	      else
	        {
	          while ((PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_EXCHANGE_LONG(
                                  (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
			          (PTW32_INTERLOCKED_LONG) -1) != 0)
		    {
	              if (WAIT_OBJECT_0 != WaitForSingleObject (mx->event, INFINITE))
		        {
	                  result = EINVAL;
		          break;
		        }
		    }

	          if (0 == result)
		    {
		      mx->recursive_count = 1;
		      mx->ownerThread = self;
		    }
	        }
	    }
        }
    }
  else
    {
      /*
       * Robust types
       * All types record the current owner thread.
       * The mutex is added to a per thread list when ownership is acquired.
       */
      ptw32_robust_state_t* statePtr = &mx->robustNode->stateInconsistent;

      if ((PTW32_INTERLOCKED_LONG)PTW32_ROBUST_NOTRECOVERABLE == PTW32_INTERLOCKED_EXCHANGE_ADD_LONG(
                                                 (PTW32_INTERLOCKED_LONGPTR)statePtr,
                                                 (PTW32_INTERLOCKED_LONG)0))
        {
          result = ENOTRECOVERABLE;
        }
      else
        {
          pthread_t self = pthread_self();

          kind = -kind - 1; /* Convert to non-robust range */
    
          if (PTHREAD_MUTEX_NORMAL == kind)
            {
              if ((PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_EXCHANGE_LONG(
                           (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                           (PTW32_INTERLOCKED_LONG) 1) != 0)
                {
                  while (0 == (result = ptw32_robust_mutex_inherit(mutex))
                           && (PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_EXCHANGE_LONG(
                                       (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                                       (PTW32_INTERLOCKED_LONG) -1) != 0)
                    {
                      if (WAIT_OBJECT_0 != WaitForSingleObject (mx->event, INFINITE))
                        {
                          result = EINVAL;
                          break;
                        }
                      if ((PTW32_INTERLOCKED_LONG)PTW32_ROBUST_NOTRECOVERABLE ==
                                  PTW32_INTERLOCKED_EXCHANGE_ADD_LONG(
                                    (PTW32_INTERLOCKED_LONGPTR)statePtr,
                                    (PTW32_INTERLOCKED_LONG)0))
                        {
                          /* Unblock the next thread */
                          SetEvent(mx->event);
                          result = ENOTRECOVERABLE;
                          break;
                        }
                    }
                }
              if (0 == result || EOWNERDEAD == result)
                {
                  /*
                   * Add mutex to the per-thread robust mutex currently-held list.
                   * If the thread terminates, all mutexes in this list will be unlocked.
                   */
                  ptw32_robust_mutex_add(mutex, self);
                }
            }
          else
            {
              if ((PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_COMPARE_EXCHANGE_LONG(
                           (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                           (PTW32_INTERLOCKED_LONG) 1,
                           (PTW32_INTERLOCKED_LONG) 0) == 0)
                {
                  mx->recursive_count = 1;
                  /*
                   * Add mutex to the per-thread robust mutex currently-held list.
                   * If the thread terminates, all mutexes in this list will be unlocked.
                   */
                  ptw32_robust_mutex_add(mutex, self);
                }
              else
                {
                  if (pthread_equal (mx->ownerThread, self))
                    {
                      if (PTHREAD_MUTEX_RECURSIVE == kind)
                        {
                          mx->recursive_count++;
                        }
                      else
                        {
                          result = EDEADLK;
                        }
                    }
                  else
                    {
                      while (0 == (result = ptw32_robust_mutex_inherit(mutex))
                               && (PTW32_INTERLOCKED_LONG) PTW32_INTERLOCKED_EXCHANGE_LONG(
                                           (PTW32_INTERLOCKED_LONGPTR) &mx->lock_idx,
                                           (PTW32_INTERLOCKED_LONG) -1) != 0)
                        {
                          if (WAIT_OBJECT_0 != WaitForSingleObject (mx->event, INFINITE))
                            {
                              result = EINVAL;
                              break;
                            }
                          if ((PTW32_INTERLOCKED_LONG)PTW32_ROBUST_NOTRECOVERABLE ==
                                      PTW32_INTERLOCKED_EXCHANGE_ADD_LONG(
                                        (PTW32_INTERLOCKED_LONGPTR)statePtr,
                                        (PTW32_INTERLOCKED_LONG)0))
                            {
                              /* Unblock the next thread */
                              SetEvent(mx->event);
                              result = ENOTRECOVERABLE;
                              break;
                            }
                        }

                      if (0 == result || EOWNERDEAD == result)
                        {
                          mx->recursive_count = 1;
                          /*
                           * Add mutex to the per-thread robust mutex currently-held list.
                           * If the thread terminates, all mutexes in this list will be unlocked.
                           */
                          ptw32_robust_mutex_add(mutex, self);
                        }
                    }
	        }
            }
        }
    }

  return (result);
}

