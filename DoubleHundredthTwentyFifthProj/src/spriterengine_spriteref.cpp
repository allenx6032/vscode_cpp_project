#include "spriterengine_spriteref.h"

#include "spriterengine_entityinstance.h"

#include "spriterengine_spriterefinstance.h"

namespace SpriterEngine
{

	SpriteRef::SpriteRef(int initialObjectId, int initialParentObjectId, TimelineKey *initialKey, int initialFileId, bool initialUseDefaultPivot) :
		ObjectRef(initialObjectId, initialParentObjectId, initialKey),
		fileId(initialFileId),
		useDefaultPivot(initialUseDefaultPivot)
	{
	}

	ObjectRefInstance *SpriteRef::getNewSpecializedObjectRefInstance(EntityInstance *entityInstance, EntityInstanceData *entityInstanceData, TransformProcessor *initialParentTransformer, UniversalObjectInterface *objectInstance)
	{
		return new SpriteRefInstance(objectInstance, initialParentTransformer, getKey(), entityInstance->getFile(fileId), useDefaultPivot);
	}

}
