#include "spriterengine_variabletimelineinstance.h"

#include "spriterengine_entityinstancedata.h"

namespace SpriterEngine
{

	VariableTimelineInstance::VariableTimelineInstance(EntityInstanceData *entityInstanceData, Timeline *timeline, int objectId, int initialVariableId) :
		TimelineInstance(timeline)
	{
		resultObject = entityInstanceData->getVariable(objectId, initialVariableId);
	}

}
