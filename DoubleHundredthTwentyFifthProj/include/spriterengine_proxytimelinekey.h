#ifndef PROXYTIMELINEKEY_H
#define PROXYTIMELINEKEY_H

#include "spriterengine_timelinekey.h"

namespace SpriterEngine
{

	class ProxyTimelineKey : public TimelineKey
	{
	public:
		ProxyTimelineKey(TimeInfo *initialTimeInfo, UniversalObjectInterface *initialObjectInfo, UniversalObjectInterface *initialNextObjectInfo);

		~ProxyTimelineKey() override;
	};

}

#endif // PROXYTIMELINEKEY_H
