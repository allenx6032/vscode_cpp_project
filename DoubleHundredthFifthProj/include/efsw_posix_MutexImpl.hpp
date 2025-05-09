#ifndef EFSW_MUTEXIMPLPOSIX_HPP
#define EFSW_MUTEXIMPLPOSIX_HPP

#include <efsw_base.hpp>

#if defined( EFSW_PLATFORM_POSIX )

#include <pthread.h>

namespace efsw { namespace Platform {

class MutexImpl {
  public:
	MutexImpl();

	~MutexImpl();

	void lock();

	void unlock();

  private:
	pthread_mutex_t mMutex;
};

}} // namespace efsw::Platform

#endif

#endif
