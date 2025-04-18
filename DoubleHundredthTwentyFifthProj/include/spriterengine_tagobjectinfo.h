#ifndef TAGOBJECTINFO_H
#define TAGOBJECTINFO_H

#include "spriterengine_universalobjectinterface.h"

#include "spriterengine_taglist.h"

namespace SpriterEngine
{

	class TagObjectInfo : public UniversalObjectInterface
	{
	public:
		TagObjectInfo();

		void setObjectToLinear(UniversalObjectInterface *bObject, real t, UniversalObjectInterface *resultObject) override;

		void pushBackTag(const std::string *tag);

	private:
		TagList tagList;
	};

}

#endif // TAGOBJECTINFO_H
