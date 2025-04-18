#include <cmath>

#include "spriterengine_bonerefinstance.h"

#include "spriterengine_universalobjectinterface.h"

namespace SpriterEngine
{

	BoneRefInstance::BoneRefInstance(UniversalObjectInterface *initialResultObject,
		TransformProcessor *initialParentTransformer,
		TransformProcessor *initialSelfTransformer,
		TimelineKey *initialKey) :
		ObjectRefInstance(initialResultObject, initialParentTransformer, initialKey),
		transformProcessor(initialSelfTransformer)
	{
	}

	void BoneRefInstance::processTransform()
	{
		ObjectRefInstance::processTransform();
		transformProcessor->setTrigFunctions();
	}
}
