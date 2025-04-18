#include "spriterengine_object.h"

#include "spriterengine_settings.h"

#include "spriterengine_objectfactory.h"

#include "spriterengine_pointobjectinfo.h"
#include "spriterengine_boneobjectinfo.h"
#include "spriterengine_boxobjectinfo.h"
#include "spriterengine_spriteobjectinfo.h"
#include "spriterengine_entityobjectinfo.h"
#include "spriterengine_soundobjectinfo.h"
#include "spriterengine_triggerobjectinfo.h"
#include "spriterengine_pointinstanceinfo.h"
#include "spriterengine_boneinstanceinfo.h"
#include "spriterengine_boxinstanceinfo.h"

#include "spriterengine_variableinstancenameandidmap.h"

#include "spriterengine_spritermodel.h"

#include "spriterengine_entityinstance.h"
#include "spriterengine_entityinstancedata.h"

namespace SpriterEngine
{

	Object::Object(std::string initialName, int initialId, ObjectType initialObjectType) :
		name(initialName),
		objectId(initialId),
		objectType(initialObjectType)
	{
	}

	std::string Object::getName()
	{
		return name;
	}

	int Object::getId()
	{
		return objectId;
	}

	Object::ObjectType Object::getType()
	{
		return objectType;
	}

	void Object::addInitializationId(int newId)
	{
		for (auto& it : initializationIds)
		{
			if (it == newId)
			{
				return;
			}
		}
		initializationIds.push_back(newId);
	}

	void Object::setSize(point newSize)
	{
		size = newSize;
	}

	UniversalObjectInterface *Object::getNewObjectInfoInstance()
	{
		switch (objectType)
		{
		case OBJECTTYPE_POINT:
			return new PointObjectInfo();

		case OBJECTTYPE_BONE:
			return new BoneObjectInfo();

		case OBJECTTYPE_BOX:
			return new BoxObjectInfo();

		case OBJECTTYPE_SPRITE:
			return new SpriteObjectInfo();

		case OBJECTTYPE_ENTITY:
			return new EntityObjectInfo();

		case OBJECTTYPE_SOUND:
			return new SoundObjectInfo();

		case OBJECTTYPE_TRIGGER:
			return new TriggerObjectInfo();

		default:
			Settings::error("Object::getNewObjectInfoInstance - invalid object type");
			return 0;
		}
	}

	void Object::addObjectInstanceToEntityInstanceData(SpriterModel *model, EntityInstance *entityInstance, EntityInstanceData *entityInstanceData, ObjectFactory *objectFactory)
	{
		switch (objectType)
		{
		case OBJECTTYPE_SPRITE:
			if (objectFactory)
			{
				entityInstanceData->setObjectInstance(objectId, name, objectFactory->newSpriteObjectinfo());
			}
			else
			{
				entityInstanceData->setObjectInstance(objectId, name, new SpriteObjectInfo());
			}
			break;

		case OBJECTTYPE_POINT:
			if (objectFactory)
			{
				entityInstanceData->setObjectInstance(objectId, name, objectFactory->newPointInstanceInfo());
			}
			else
			{
				entityInstanceData->setObjectInstance(objectId, name, new PointInstanceInfo());
			}
			break;

		case OBJECTTYPE_BONE:
			if (objectFactory)
			{
				entityInstanceData->setObjectInstance(objectId, name, objectFactory->newBoneInstanceInfo(size));
			}
			else
			{
				entityInstanceData->setObjectInstance(objectId, name, new BoneInstanceInfo(size));
			}
			break;

		case OBJECTTYPE_BOX:
			if (objectFactory)
			{
				entityInstanceData->setObjectInstance(objectId, name, objectFactory->newBoxInstanceInfo(size));
			}
			else
			{
				entityInstanceData->setObjectInstance(objectId, name, new BoxInstanceInfo(size));
			}
			break;

		case OBJECTTYPE_ENTITY:
			entityInstanceData->setObjectInstance(objectId, name, model->getNewEntityInstance(&initializationIds));
			break;

		case OBJECTTYPE_SOUND:
			if (initializationIds.size())
			{
				entityInstanceData->setSoundInstance(objectId, name, entityInstance->getFile(initializationIds.back()));
			}
			break;

		case OBJECTTYPE_TRIGGER:
			if (objectFactory)
			{
				entityInstanceData->setTriggerInstance(objectId, name, objectFactory->newTriggerObjectInfo(name));
			}
			else
			{
				entityInstanceData->setTriggerInstance(objectId, name);
			}
			break;

		default:
			entityInstanceData->setObjectInstance(objectId, name, getNewObjectInfoInstance());
			break;
		}
	}

}
