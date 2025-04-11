#include <efsw_Mutex.hpp>
#include <efsw_platformimpl.hpp>

namespace efsw {

Mutex::Mutex() : mMutexImpl( new Platform::MutexImpl() ) {}

Mutex::~Mutex() {
	efSAFE_DELETE( mMutexImpl );
}

void Mutex::lock() {
	mMutexImpl->lock();
}

void Mutex::unlock() {
	mMutexImpl->unlock();
}

} // namespace efsw
