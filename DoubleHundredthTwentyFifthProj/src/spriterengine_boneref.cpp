#include "spriterengine_boneref.h"

#include "spriterengine_entityinstancedata.h"

#include "spriterengine_bonerefinstance.h"

namespace SpriterEngine
{

	BoneRef::BoneRef(int initialObjectId, int initialParentObjectId, TimelineKey *initialKey) :
		ObjectRef(initialObjectId, initialParentObjectId, initialKey)
	{
	}

	ObjectRefInstance *BoneRef::getNewSpecializedObjectRefInstance(EntityInstance *entityInstance, EntityInstanceData *entityInstanceData, TransformProcessor *initialParentTransformer, UniversalObjectInterface *objectInstance)
	{
		return new BoneRefInstance(objectInstance, initialParentTransformer, entityInstanceData->getTransformer(id()), getKey());
	}

}
