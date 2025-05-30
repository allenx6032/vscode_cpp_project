/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include "curl_setup.h"

#include <limits.h>

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#elif defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#if !defined(HAVE_SELECT) && !defined(HAVE_POLL_FINE)
#error "We can't compile without select() or poll() support."
#endif

#if defined(__BEOS__) && !defined(__HAIKU__)
/* BeOS has FD_SET defined in socket.h */
#include <socket.h>
#endif

#ifdef MSDOS
#include <dos.h>  /* delay() */
#endif

#ifdef __VXWORKS__
#include <strings.h>  /* bzero() in FD_SET */
#endif

#include <curl.h>

#include "curl_urldata.h"
#include "curl_connect.h"
#include "curl_select.h"
#include "curl_timeval.h"
#include "curl_warnless.h"

/*
 * Internal function used for waiting a specific amount of ms
 * in Curl_socket_check() and Curl_poll() when no file descriptor
 * is provided to wait on, just being used to delay execution.
 * WinSock select() and poll() timeout mechanisms need a valid
 * socket descriptor in a not null file descriptor set to work.
 * Waiting indefinitely with this function is not allowed, a
 * zero or negative timeout value will return immediately.
 * Timeout resolution, accuracy, as well as maximum supported
 * value is system dependent, neither factor is a citical issue
 * for the intended use of this function in the library.
 *
 * Return values:
 *   -1 = system call error, invalid timeout value, or interrupted
 *    0 = specified timeout has elapsed
 */
int Curl_wait_ms(timediff_t timeout_ms)
{
  int r = 0;

  if(!timeout_ms)
    return 0;
  if(timeout_ms < 0) {
    SET_SOCKERRNO(EINVAL);
    return -1;
  }
#if defined(MSDOS)
  delay(timeout_ms);
#elif defined(WIN32)
  /* prevent overflow, timeout_ms is typecast to ULONG/DWORD. */
#if TIMEDIFF_T_MAX >= ULONG_MAX
  if(timeout_ms >= ULONG_MAX)
    timeout_ms = ULONG_MAX-1;
    /* don't use ULONG_MAX, because that is equal to INFINITE */
#endif
  Sleep((ULONG)timeout_ms);
#else
#if defined(HAVE_POLL_FINE)
  /* prevent overflow, timeout_ms is typecast to int. */
#if TIMEDIFF_T_MAX > INT_MAX
  if(timeout_ms > INT_MAX)
    timeout_ms = INT_MAX;
#endif
  r = poll(NULL, 0, (int)timeout_ms);
#else
  {
    struct timeval pending_tv;
    timediff_t tv_sec = timeout_ms / 1000;
    timediff_t tv_usec = (timeout_ms % 1000) * 1000; /* max=999999 */
#ifdef HAVE_SUSECONDS_T
#if TIMEDIFF_T_MAX > TIME_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    if(tv_sec > TIME_T_MAX)
      tv_sec = TIME_T_MAX;
#endif
    pending_tv.tv_sec = (time_t)tv_sec;
    pending_tv.tv_usec = (suseconds_t)tv_usec;
#else
#if TIMEDIFF_T_MAX > INT_MAX
    /* tv_sec overflow check in case time_t is signed */
    if(tv_sec > INT_MAX)
      tv_sec = INT_MAX;
#endif
    pending_tv.tv_sec = (int)tv_sec;
    pending_tv.tv_usec = (int)tv_usec;
#endif
    r = select(0, NULL, NULL, NULL, &pending_tv);
  }
#endif /* HAVE_POLL_FINE */
#endif /* USE_WINSOCK */
  if(r)
    r = -1;
  return r;
}

/*
 * This is a wrapper around select() to aid in Windows compatibility.
 * A negative timeout value makes this function wait indefinitely,
 * unless no valid file descriptor is given, when this happens the
 * negative timeout is ignored and the function times out immediately.
 *
 * Return values:
 *   -1 = system call error or fd >= FD_SETSIZE
 *    0 = timeout
 *    N = number of signalled file descriptors
 */
int Curl_select(curl_socket_t maxfd,   /* highest socket number */
                curl_fd_set *fds_read,      /* sockets ready for reading */
                curl_fd_set *fds_write,     /* sockets ready for writing */
                curl_fd_set *fds_err,       /* sockets with errors */
                timediff_t timeout_ms) /* milliseconds to wait */
{
  struct timeval pending_tv;
  struct timeval *ptimeout;
  int r;

#ifdef USE_WINSOCK
  /* WinSock select() can't handle zero events.  See the comment below. */
  if((!fds_read || fds_read->fd_count == 0) &&
     (!fds_write || fds_write->fd_count == 0) &&
     (!fds_err || fds_err->fd_count == 0)) {
    r = Curl_wait_ms(timeout_ms);
    return r;
  }
#endif

  ptimeout = &pending_tv;
  if(timeout_ms < 0) {
    ptimeout = NULL;
  }
  else if(timeout_ms > 0) {
    timediff_t tv_sec = timeout_ms / 1000;
    timediff_t tv_usec = (timeout_ms % 1000) * 1000; /* max=999999 */
#ifdef HAVE_SUSECONDS_T
#if TIMEDIFF_T_MAX > TIME_T_MAX
    /* tv_sec overflow check in case time_t is signed */
    if(tv_sec > TIME_T_MAX)
      tv_sec = TIME_T_MAX;
#endif
    pending_tv.tv_sec = (time_t)tv_sec;
    pending_tv.tv_usec = (suseconds_t)tv_usec;
#elif defined(WIN32) /* maybe also others in the future */
#if TIMEDIFF_T_MAX > LONG_MAX
    /* tv_sec overflow check on Windows there we know it is long */
    if(tv_sec > LONG_MAX)
      tv_sec = LONG_MAX;
#endif
    pending_tv.tv_sec = (long)tv_sec;
    pending_tv.tv_usec = (long)tv_usec;
#else
#if TIMEDIFF_T_MAX > INT_MAX
    /* tv_sec overflow check in case time_t is signed */
    if(tv_sec > INT_MAX)
      tv_sec = INT_MAX;
#endif
    pending_tv.tv_sec = (int)tv_sec;
    pending_tv.tv_usec = (int)tv_usec;
#endif
  }
  else {
    pending_tv.tv_sec = 0;
    pending_tv.tv_usec = 0;
  }

#ifdef USE_WINSOCK
  /* WinSock select() must not be called with an fd_set that contains zero
    fd flags, or it will return WSAEINVAL.  But, it also can't be called
    with no fd_sets at all!  From the documentation:

    Any two of the parameters, readfds, writefds, or exceptfds, can be
    given as null. At least one must be non-null, and any non-null
    descriptor set must contain at least one handle to a socket.

    It is unclear why WinSock doesn't just handle this for us instead of
    calling this an error.
  */
  r = select((int)maxfd + 1,
             fds_read && fds_read->fd_count ? fds_read : NULL,
             fds_write && fds_write->fd_count ? fds_write : NULL,
             fds_err && fds_err->fd_count ? fds_err : NULL, ptimeout);
#elif defined(FreeRTOS)
  {
    /* convert the timeout to FreeRTOS ticks */
    TickType_t ticks = ptimeout ? ptimeout->tv_sec * configTICK_RATE_HZ : +
      (ptimeout->tv_usec * configTICK_RATE_HZ) / 1000000;
    r = FreeRTOS_select(fds_read, ticks);
  }
#else
  r = select((int)maxfd + 1, fds_read, fds_write, fds_err, ptimeout);
#endif

  return r;
}

/*
 * Wait for read or write events on a set of file descriptors. It uses poll()
 * when a fine poll() is available, in order to avoid limits with FD_SETSIZE,
 * otherwise select() is used.  An error is returned if select() is being used
 * and a file descriptor is too large for FD_SETSIZE.
 *
 * A negative timeout value makes this function wait indefinitely,
 * unless no valid file descriptor is given, when this happens the
 * negative timeout is ignored and the function times out immediately.
 *
 * Return values:
 *   -1 = system call error or fd >= FD_SETSIZE
 *    0 = timeout
 *    [bitmask] = action as described below
 *
 * CURL_CSELECT_IN - first socket is readable
 * CURL_CSELECT_IN2 - second socket is readable
 * CURL_CSELECT_OUT - write socket is writable
 * CURL_CSELECT_ERR - an error condition occurred
 */
int Curl_socket_check(curl_socket_t readfd0, /* two sockets to read from */
                      curl_socket_t readfd1,
                      curl_socket_t writefd, /* socket to write to */
                      timediff_t timeout_ms) /* milliseconds to wait */
{
#ifdef HAVE_POLL_FINE
  struct pollfd pfd[3];
  int num;
#else
  struct timeval pending_tv;
  struct timeval *ptimeout;
  curl_fd_set fds_read;
  curl_fd_set fds_write;
  curl_fd_set fds_err;
  curl_socket_t maxfd;
#endif
  int r;
  int ret;

  if((readfd0 == CURL_SOCKET_BAD) && (readfd1 == CURL_SOCKET_BAD) &&
     (writefd == CURL_SOCKET_BAD)) {
    /* no sockets, just wait */
    r = Curl_wait_ms(timeout_ms);
    return r;
  }

  /* Avoid initial timestamp, avoid Curl_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL_FINE

  num = 0;
  if(readfd0 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd0;
    pfd[num].events = POLLRDNORM|POLLIN|POLLRDBAND|POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  if(readfd1 != CURL_SOCKET_BAD) {
    pfd[num].fd = readfd1;
    pfd[num].events = POLLRDNORM|POLLIN|POLLRDBAND|POLLPRI;
    pfd[num].revents = 0;
    num++;
  }
  if(writefd != CURL_SOCKET_BAD) {
    pfd[num].fd = writefd;
    pfd[num].events = POLLWRNORM|POLLOUT;
    pfd[num].revents = 0;
    num++;
  }

  r = Curl_poll(pfd, num, timeout_ms);
  if(r <= 0)
    return r;

  ret = 0;
  num = 0;
  if(readfd0 != CURL_SOCKET_BAD) {
    if(pfd[num].revents & (POLLRDNORM|POLLIN|POLLERR|POLLHUP))
      ret |= CURL_CSELECT_IN;
    if(pfd[num].revents & (POLLRDBAND|POLLPRI|POLLNVAL))
      ret |= CURL_CSELECT_ERR;
    num++;
  }
  if(readfd1 != CURL_SOCKET_BAD) {
    if(pfd[num].revents & (POLLRDNORM|POLLIN|POLLERR|POLLHUP))
      ret |= CURL_CSELECT_IN2;
    if(pfd[num].revents & (POLLRDBAND|POLLPRI|POLLNVAL))
      ret |= CURL_CSELECT_ERR;
    num++;
  }
  if(writefd != CURL_SOCKET_BAD) {
    if(pfd[num].revents & (POLLWRNORM|POLLOUT))
      ret |= CURL_CSELECT_OUT;
    if(pfd[num].revents & (POLLERR|POLLHUP|POLLNVAL))
      ret |= CURL_CSELECT_ERR;
  }

  return ret;

#else  /* HAVE_POLL_FINE */

#if defined(FreeRTOS)
  /* using its weirdo select() */
  fds_read = FreeRTOS_CreateSocketSet();
  maxfd = (curl_socket_t)-1;
  if(readfd0 != CURL_SOCKET_BAD)
    CURL_FD_SET(readfd0, &fds_read, eSELECT_READ);
  if(readfd1 != CURL_SOCKET_BAD)
    CURL_FD_SET(readfd1, &fds_read, eSELECT_READ);
  if(writefd != CURL_SOCKET_BAD)
    CURL_FD_SET(writefd, &fds_read, eSELECT_WRITE);

#else  /* HAVE_POLL_FINE */

  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  FD_ZERO(&fds_read);
  if(readfd0 != CURL_SOCKET_BAD) {
    VERIFY_SOCK(readfd0);
    FD_SET(readfd0, &fds_read);
    FD_SET(readfd0, &fds_err);
    maxfd = readfd0;
  }
  if(readfd1 != CURL_SOCKET_BAD) {
    VERIFY_SOCK(readfd1);
    FD_SET(readfd1, &fds_read);
    FD_SET(readfd1, &fds_err);
    if(readfd1 > maxfd)
      maxfd = readfd1;
  }

  FD_ZERO(&fds_write);
  if(writefd != CURL_SOCKET_BAD) {
    VERIFY_SOCK(writefd);
    FD_SET(writefd, &fds_write);
    FD_SET(writefd, &fds_err);
    if(writefd > maxfd)
      maxfd = writefd;
  }
#endif

  /* We know that we have at least one bit set in at least two fd_sets in
     this case, but we may have no bits set in either fds_read or fd_write,
     so check for that and handle it.  Luckily, with WinSock, we can _also_
     ask how many bits are set on an fd_set.

     Note also that WinSock ignores the first argument, so we don't worry
     about the fact that maxfd is computed incorrectly with WinSock (since
     curl_socket_t is unsigned in such cases and thus -1 is the largest
     value).
  */
  r = Curl_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);

  if(r < 0)
    return -1;
  if(r == 0)
    return 0;

  ret = 0;
#ifdef FreeRTOS
  if(r) {
    if((readfd0 != CURL_SOCKET_BAD) && FreeRTOS_FD_ISSET(readfd0, fds_read))
      ret |= CURL_CSELECT_IN;
    if((readfd1 != CURL_SOCKET_BAD) && FreeRTOS_FD_ISSET(readfd1, fds_read))
      ret |= CURL_CSELECT_IN2;
    if((writefd != CURL_SOCKET_BAD) && FreeRTOS_FD_ISSET(writefd, fds_read))
      ret |= CURL_CSELECT_OUT;
  }
#else
  if(readfd0 != CURL_SOCKET_BAD) {
    if(FD_ISSET(readfd0, &fds_read))
      ret |= CURL_CSELECT_IN;
    if(FD_ISSET(readfd0, &fds_err))
      ret |= CURL_CSELECT_ERR;
  }
  if(readfd1 != CURL_SOCKET_BAD) {
    if(FD_ISSET(readfd1, &fds_read))
      ret |= CURL_CSELECT_IN2;
    if(FD_ISSET(readfd1, &fds_err))
      ret |= CURL_CSELECT_ERR;
  }
  if(writefd != CURL_SOCKET_BAD) {
    if(FD_ISSET(writefd, &fds_write))
      ret |= CURL_CSELECT_OUT;
    if(FD_ISSET(writefd, &fds_err))
      ret |= CURL_CSELECT_ERR;
  }
#endif

  return ret;

#endif  /* HAVE_POLL_FINE */
}

/*
 * This is a wrapper around poll().  If poll() does not exist, then
 * select() is used instead.  An error is returned if select() is
 * being used and a file descriptor is too large for FD_SETSIZE.
 * A negative timeout value makes this function wait indefinitely,
 * unless no valid file descriptor is given, when this happens the
 * negative timeout is ignored and the function times out immediately.
 *
 * Return values:
 *   -1 = system call error or fd >= FD_SETSIZE
 *    0 = timeout
 *    N = number of structures with non zero revent fields
 */
int Curl_poll(struct pollfd ufds[], unsigned int nfds, timediff_t timeout_ms)
{
#ifdef HAVE_POLL_FINE
  int pending_ms;
#else
  curl_fd_set fds_read;
  curl_fd_set fds_write;
  curl_fd_set fds_err;
#endif
  curl_socket_t maxfd;
  bool fds_none = TRUE;
  unsigned int i;
  int r;

  if(ufds) {
    for(i = 0; i < nfds; i++) {
      if(ufds[i].fd != CURL_SOCKET_BAD) {
        fds_none = FALSE;
        break;
      }
    }
  }
  if(fds_none) {
    /* no sockets, just wait */
    r = Curl_wait_ms(timeout_ms);
    return r;
  }

  /* Avoid initial timestamp, avoid Curl_now() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

#ifdef HAVE_POLL_FINE
  if(timeout_ms > 0)
    pending_ms = (int)timeout_ms;
  else if(timeout_ms < 0)
    pending_ms = -1;
  else
    pending_ms = 0;
  r = poll(ufds, nfds, pending_ms);

  if(r < 0)
    return -1;
  if(r == 0)
    return 0;

  for(i = 0; i < nfds; i++) {
    if(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    if(ufds[i].revents & POLLHUP)
      ufds[i].revents |= POLLIN;
    if(ufds[i].revents & POLLERR)
      ufds[i].revents |= (POLLIN|POLLOUT);
  }

#else  /* HAVE_POLL_FINE */

#ifdef FreeRTOS
  fds_read = FreeRTOS_CreateSocketSet();
  maxfd = (curl_socket_t)-1;

  for(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    if(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    VERIFY_SOCK(ufds[i].fd);
    if(ufds[i].events & (POLLIN|POLLOUT|POLLPRI|
                          POLLRDNORM|POLLWRNORM|POLLRDBAND)) {
      if(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      if(ufds[i].events & (POLLRDNORM|POLLIN))
        CURL_FD_SET(ufds[i].fd, &fds_read, eSELECT_READ);
      if(ufds[i].events & (POLLWRNORM|POLLOUT))
        CURL_FD_SET(ufds[i].fd, &fds_read, eSELECT_WRITE);
    }
  }
#else
  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (curl_socket_t)-1;

  for(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    if(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
    VERIFY_SOCK(ufds[i].fd);
    if(ufds[i].events & (POLLIN|POLLOUT|POLLPRI|
                          POLLRDNORM|POLLWRNORM|POLLRDBAND)) {
      if(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      if(ufds[i].events & (POLLRDNORM|POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      if(ufds[i].events & (POLLWRNORM|POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      if(ufds[i].events & (POLLRDBAND|POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }
#endif

  r = Curl_select(maxfd, &fds_read, &fds_write, &fds_err, timeout_ms);

  if(r < 0)
    return -1;
  if(r == 0)
    return 0;

  r = 0;
  for(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    if(ufds[i].fd == CURL_SOCKET_BAD)
      continue;
#ifdef FreeRTOS
    if((ufds[i].events & POLLIN) && FreeRTOS_FD_ISSET(ufds[i].fd, fds_read))
      ufds[i].revents |= POLLIN;
    if((ufds[i].events & POLLOUT) && FreeRTOS_FD_ISSET(ufds[i].fd, fds_read))
      ufds[i].revents |= POLLOUT;
#else
    if(FD_ISSET(ufds[i].fd, &fds_read))
      ufds[i].revents |= POLLIN;
    if(FD_ISSET(ufds[i].fd, &fds_write))
      ufds[i].revents |= POLLOUT;
    if(FD_ISSET(ufds[i].fd, &fds_err))
      ufds[i].revents |= POLLPRI;
#endif
    if(ufds[i].revents != 0)
      r++;
  }

#endif  /* HAVE_POLL_FINE */

  return r;
}

#ifdef TPF
/*
 * This is a replacement for select() on the TPF platform.
 * It is used whenever libcurl calls select().
 * The call below to tpf_process_signals() is required because
 * TPF's select calls are not signal interruptible.
 *
 * Return values are the same as select's.
 */
int tpf_select_libcurl(int maxfds, fd_set *reads, fd_set *writes,
                       fd_set *excepts, struct timeval *tv)
{
   int rc;

   rc = tpf_select_bsd(maxfds, reads, writes, excepts, tv);
   tpf_process_signals();
   return rc;
}
#endif /* TPF */
