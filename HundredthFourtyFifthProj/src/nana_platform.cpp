/*
 *	A platform API implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/system/platform.cpp
 *	@description:
 *		this implements some API for platform-independent programming
 */
#include <nana_deploy.hpp>

#if defined(NANA_WINDOWS)
	#include <windows.h>
	#include PLATFORM_SPEC_HPP
#elif defined(NANA_LINUX)
	#include <time.h>
	#include <errno.h>
	#include <unistd.h>
	#include <sys/time.h>
	#include <sys/syscall.h>
#endif

namespace nana
{
namespace system
{
	//sleep
	//@brief:	Suspend current thread for a specified milliseconds.
	//			its precision is depended on hardware.
	void sleep(unsigned milliseconds)
	{
#if defined(NANA_WINDOWS)
		::Sleep(milliseconds);
#elif defined(NANA_LINUX)
		struct timespec timeOut, remains;
		timeOut.tv_sec = milliseconds / 1000;
		timeOut.tv_nsec = (milliseconds % 1000) * 1000000;
		while(-1 == ::nanosleep(&timeOut, &remains))
		{
			if(errno == EINTR)
				timeOut = remains;
			else
				break;
		}
#endif
	}

	//this_thread_id
	//@brief: get the identifier of calling thread.
	unsigned long this_thread_id()
	{
#if defined(NANA_WINDOWS)
		return ::GetCurrentThreadId();
#elif defined(NANA_LINUX)
		return ::syscall(__NR_gettid);
#endif
	}

	unsigned long timestamp()
	{
#if defined(NANA_WINDOWS)
		return ::GetTickCount();
#elif defined(NANA_LINUX)
		struct timeval tv;
		::gettimeofday(&tv, 0);
		return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
	}

	bool get_async_mouse_state(int button)
	{
#if defined(NANA_WINDOWS)
		bool swap = (::GetSystemMetrics(SM_SWAPBUTTON) != 0);
		switch(button)
		{
		case 1: //Right
			button = swap ? VK_LBUTTON : VK_RBUTTON;
			break;
		case 2:
			button = VK_MBUTTON;
			break;
		default:
			button = swap ? VK_RBUTTON : VK_LBUTTON;
			break;
		}

		return (::GetAsyncKeyState(button) != 0);
#elif defined(NANA_LINUX)
		return false;
#endif
	}

	//open an url through a default browser
	void open_url(const nana::string& url)
	{
		if(url.empty())
			return;

#if defined(NANA_WINDOWS)
		if(::ShellExecute(0, STR("open"), url.c_str(), 0, 0, SW_SHOWNORMAL) < reinterpret_cast<HINSTANCE>(32))
		{
			//Because ShellExecute can delegate execution to Shell extensions (data sources, context menu handlers,
			//verb implementations) that are activated using Component Object Model (COM), COM should be initialized
			//before ShellExecute is called. Some Shell extensions require the COM single-threaded apartment (STA) type.
			//In that case, COM should be initialized under WinXP.
			nana::detail::platform_spec::co_initializer co_init;
			::ShellExecute(0, STR("open"), url.c_str(), 0, 0, SW_SHOWNORMAL);
		}
#elif defined(NANA_LINUX)
#endif
	}
}//end namespace system
}//end namespace nana
