#ifndef TRANSFORMPROCESSOR_H
#define TRANSFORMPROCESSOR_H

#include "spriterengine_global.h"

namespace SpriterEngine
{

	class UniversalObjectInterface;

	class TransformProcessor
	{
	public:
		TransformProcessor(UniversalObjectInterface *hostParentObject);

		void setTrigFunctions();
		void transformChildObject(UniversalObjectInterface *childObject) const;

	private:
		UniversalObjectInterface *parentObject;
		real angleSin;
		real angleCos;
	};

}

#endif // TRANSFORMPROCESSOR_H
