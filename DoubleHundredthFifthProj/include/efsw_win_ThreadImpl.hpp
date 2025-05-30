#ifndef EFSW_THREADIMPLWIN_HPP
#define EFSW_THREADIMPLWIN_HPP

#include <efsw_base.hpp>

#if EFSW_PLATFORM == EFSW_PLATFORM_WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <process.h>
#include <windows.h>

namespace efsw {

class Thread;

namespace Platform {

class ThreadImpl {
  public:
	explicit ThreadImpl( efsw::Thread* owner );

	~ThreadImpl();

	void wait();

	void terminate();

  protected:
	static unsigned int __stdcall entryPoint( void* userData );

	HANDLE mThread;
	unsigned int mThreadId;
};

} // namespace Platform
} // namespace efsw

#endif

#endif
