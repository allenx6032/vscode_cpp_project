#ifndef BONEREFINSTANCE_H
#define BONEREFINSTANCE_H

#include "spriterengine_objectrefinstance.h"

#include "spriterengine_transformprocessor.h"

namespace SpriterEngine
{

	class BoneRefInstance : public ObjectRefInstance
	{
	public:
		BoneRefInstance(UniversalObjectInterface *initialResultObject,
			TransformProcessor *initialParentTransformer,
			TransformProcessor *initialSelfTransformer,
			TimelineKey *initialKey);

		void processTransform() override;

	private:
		TransformProcessor *transformProcessor;
	};

}

#endif // BONEREFINSTANCE_H
