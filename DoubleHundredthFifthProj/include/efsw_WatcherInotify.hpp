#ifndef EFSW_WATCHERINOTIFY_HPP
#define EFSW_WATCHERINOTIFY_HPP

#include <efsw_FileInfo.hpp>
#include <efsw_FileWatcherImpl.hpp>

namespace efsw {

class WatcherInotify : public Watcher {
  public:
	WatcherInotify();

	bool inParentTree( WatcherInotify* parent );

	WatcherInotify* Parent;
	WatchID InotifyID;

	FileInfo DirInfo;
	bool syntheticEvents{ false };
};

} // namespace efsw

#endif
