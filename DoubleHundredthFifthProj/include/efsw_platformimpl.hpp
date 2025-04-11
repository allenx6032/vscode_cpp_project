#ifndef EFSW_PLATFORMIMPL_HPP
#define EFSW_PLATFORMIMPL_HPP

#include <efsw_base.hpp>

#if defined( EFSW_PLATFORM_POSIX )
#include <efsw_posix_ThreadImpl.hpp>
#include <efsw_posix_MutexImpl.hpp>
#include <efsw_posix_SystemImpl.hpp>
#include <efsw_posix_FileSystemImpl.hpp>
#elif EFSW_PLATFORM == EFSW_PLATFORM_WIN32
#include <efsw_win_ThreadImpl.hpp>
#include <efsw_win_MutexImpl.hpp>
#include <efsw_win_SystemImpl.hpp>
#include <efsw_win_FileSystemImpl.hpp>
#else
#error Thread, Mutex, and System not implemented for this platform.
#endif

#endif
