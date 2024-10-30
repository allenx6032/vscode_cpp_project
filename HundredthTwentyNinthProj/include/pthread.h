/*
 * Copyright (c) 2007-2009 Marko Kreen
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/** @file
 *
 * Pthreads compat for win32.
 */
#ifndef _USUAL_PTHREAD_H_
#define _USUAL_PTHREAD_H_

#ifdef __cplusplus
extern "C"{
#endif


#include <Windows.h>

typedef HANDLE pthread_t;
typedef HANDLE pthread_mutex_t;
typedef int pthread_attr_t;

int pthread_create(pthread_t *t, pthread_attr_t *attr, void *(*fn)(void *), void *arg);
int pthread_mutex_init(pthread_mutex_t *lock, void *unused);
int pthread_mutex_destroy(pthread_mutex_t *lock);
int pthread_mutex_lock(pthread_mutex_t *lock);
int pthread_mutex_unlock(pthread_mutex_t *lock);
int pthread_join(pthread_t *t, void **ret);

#ifdef __cplusplus
}
#endif

#endif /* HAVE_PTHREAD_H */

