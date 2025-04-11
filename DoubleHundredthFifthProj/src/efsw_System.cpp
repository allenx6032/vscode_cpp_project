#include <efsw_System.hpp>
#include <efsw_platformimpl.hpp>

namespace efsw {

void System::sleep( const unsigned long& ms ) {
	Platform::System::sleep( ms );
}

std::string System::getProcessPath() {
	return Platform::System::getProcessPath();
}

void System::maxFD() {
	Platform::System::maxFD();
}

Uint64 System::getMaxFD() {
	return Platform::System::getMaxFD();
}

} // namespace efsw
