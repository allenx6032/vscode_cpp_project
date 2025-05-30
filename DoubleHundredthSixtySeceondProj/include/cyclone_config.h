﻿/*
Copyright(C) thecodeway.com
*/
#ifndef _CYCLONE_CONFIG_H_
#define _CYCLONE_CONFIG_H_

#define CY_HAVE_UNISTD_H 1
#define CY_HAVE_INTTYPES_H 1
/* #undef CY_HAVE_SYS_UIO_H */
/* #undef CY_HAVE_SYS_EVENTFD_H */
/* #undef CY_HAVE_JEMALLOC_LIBRARIES */

/* #undef CY_SYS_LINUX */
#define CY_SYS_WINDOWS 1
/* #undef CY_SYS_MACOS */
/* #undef CY_SYS_ANDROID */

/* #undef CY_HAVE_EPOLL */
/* #undef CY_HAVE_KQUEUE */
/* #undef CY_HAVE_READWRITE_V */
/* #undef CY_HAVE_PIPE2 */
/* #undef CY_HAVE_TIMERFD */
#define CY_HAVE_TIMED_LOCK 1

#define CY_ENABLE_LOG 1
/* #undef CY_ENABLE_DEBUG */

#define CY_POLL_EPOLL   1
#define CY_POLL_KQUEUE  2
#define CY_POLL_SELECT  3

#ifdef CY_HAVE_EPOLL
#define CY_POLL_TECH CY_POLL_EPOLL
#elif defined CY_HAVE_KQUEUE
#define CY_POLL_TECH CY_POLL_KQUEUE
#else
#define CY_POLL_TECH CY_POLL_SELECT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#ifdef CY_HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef CY_HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef CY_HAVE_SYS_TYPES
#include <sys/types.h>
#endif

#ifdef CY_HAVE_JEMALLOC_LIBRARIES
#include <jemalloc/jemalloc.h>
#define CY_MALLOC je_malloc
#define CY_FREE je_free
#else
#define CY_MALLOC malloc
#define CY_FREE free
#endif

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <algorithm>

#ifdef CY_SYS_WINDOWS
	#define FD_SETSIZE (1024)
	#include <WinSock2.h>
	#include <Ws2tcpip.h>
	#include <Windows.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#if defined (__LP64__) || defined (__64BIT__) || defined (_LP64) || (__WORDSIZE == 64) || defined(_WIN64)
	#define CY_64BIT 1
#endif

#ifdef CY_SYS_WINDOWS
	#undef min
	#undef max
	typedef SOCKET	socket_t;
#ifdef MSVC 
	typedef int32_t pid_t;
#endif

	#ifdef  _WIN64
		typedef signed __int64    ssize_t;
	#else
		typedef _W64 signed int   ssize_t;
	#endif
#else
	typedef int		socket_t;
	#ifndef INVALID_SOCKET
		#define INVALID_SOCKET	(-1)
	#endif
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR	(-1)
	#endif	
#endif

#ifndef MAX_PATH
	#define MAX_PATH	(260)
#endif

#ifdef CY_SYS_WINDOWS
#define INLINE inline
#else
#define INLINE __inline
#endif

//wait c++17 support
#define CONSTEXPR 

class noncopyable
{  
protected:  
    noncopyable() {}  
    ~noncopyable() {}  
private:
    noncopyable( const noncopyable& ) = delete;  
    const noncopyable& operator=(const noncopyable&) = delete;  
};

#endif

