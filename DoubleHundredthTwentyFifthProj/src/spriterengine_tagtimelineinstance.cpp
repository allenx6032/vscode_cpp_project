#include "spriterengine_tagtimelineinstance.h"

#include "spriterengine_entityinstancedata.h"
#include "spriterengine_tagobjectinforeference.h"

#include "spriterengine_timeline.h"

namespace SpriterEngine
{

	TagTimelineInstance::TagTimelineInstance(EntityInstanceData *entityInstanceData, Timeline *timeline, int objectId) :
		TimelineInstance(timeline)
	{
		resultObject = entityInstanceData->getTags(objectId);
	}

}
