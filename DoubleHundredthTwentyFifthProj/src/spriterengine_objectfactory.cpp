#include "spriterengine_objectfactory.h"

#include "spriterengine_pointinstanceinfo.h"
#include "spriterengine_boneinstanceinfo.h"
#include "spriterengine_boxinstanceinfo.h"
#include "spriterengine_triggerobjectinfo.h"
#include "spriterengine_spriteobjectinfo.h"

namespace SpriterEngine
{

	ObjectFactory::ObjectFactory()
	{
	}

	PointInstanceInfo * ObjectFactory::newPointInstanceInfo()
	{
		return new PointInstanceInfo();
	}

	BoneInstanceInfo * ObjectFactory::newBoneInstanceInfo(point size)
	{
		return new BoneInstanceInfo(size);
	}

	BoxInstanceInfo * ObjectFactory::newBoxInstanceInfo(point size)
	{
		return new BoxInstanceInfo(size);
	}

	TriggerObjectInfo * ObjectFactory::newTriggerObjectInfo(std::string newTriggerName)
	{
		return new TriggerObjectInfo();
	}

	SpriteObjectInfo *ObjectFactory::newSpriteObjectinfo()
	{
		return new SpriteObjectInfo();
	}
}