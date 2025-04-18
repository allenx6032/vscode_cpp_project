#include "spriterengine_spriterefinstance.h"

#include "spriterengine_universalobjectinterface.h"
#include "spriterengine_filereference.h"
#include "spriterengine_imagefile.h"

namespace SpriterEngine
{

	SpriteRefInstance::SpriteRefInstance(UniversalObjectInterface *initialResultObject,
		TransformProcessor *initialParentTransformer,
		TimelineKey *initialKey,
		FileReference *initialImageRef,
		bool initialUseDefaultPivotPoint) :
		ObjectRefInstance(initialResultObject, initialParentTransformer, initialKey),
		imageRef(initialImageRef),
		useDefaultPivot(initialUseDefaultPivotPoint)
	{
	}

	void SpriteRefInstance::preProcess()
	{
		resultObject()->setImage(imageRef->image());
		if (useDefaultPivot && imageRef->image())
		{
			resultObject()->setPivot(imageRef->image()->defaultPivot);
		}
	}

	void SpriteRefInstance::process(real currentTime)
	{
		preProcess();
		ObjectRefInstance::process(currentTime);	
	}

}
