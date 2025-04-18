#include "spriterengine_variable.h"

#include "spriterengine_settings.h"

#include "spriterengine_stringvariableinfo.h"
#include "spriterengine_stringvariableinforeference.h"
#include "spriterengine_intvariableinfo.h"
#include "spriterengine_realvariableinfo.h"

#include "spriterengine_animation.h"

#include "spriterengine_timeinfo.h"
#include "spriterengine_instanteasingcurve.h"

namespace SpriterEngine
{

	Variable::Variable(std::string initialName, VariableType initialVariableType) :
		name(initialName),
		variableType(initialVariableType),
		defaultValue(getNewObjectInfoInstance())
	{
	}

	Variable::~Variable()
	{
		delete defaultValue;
	}

	std::string Variable::getName() const
	{
		return name;
	}

	UniversalObjectInterface *Variable::getNewObjectInfoInstance(bool forEntityInstance) const
	{
		switch (variableType)
		{
		case VARIABLETYPE_STRING:
			if (forEntityInstance)
			{
				return new StringVariableInfoReference();
			}
			else
			{
				return new StringVariableInfo();
			}

		case VARIABLETYPE_INT:
			return new IntVariableInfo();

		case VARIABLETYPE_REAL:
			return new RealVariableInfo();

		default:
			Settings::error("Variable::setupDefaultInAnimation - invalid variable type");
			return 0;
		}
	}

	UniversalObjectInterface *Variable::getDefaultValue() const
	{
		return defaultValue;
	}

	Variable::VariableType Variable::getType()
	{
		return variableType;
	}

	void Variable::setupDefaultInAnimation(Animation * animation, int objectId, int variableId)
	{
		Timeline *newVariableTimeline = animation->setVariableTimeline(objectId, variableId);
		if (newVariableTimeline)
		{
			UniversalObjectInterface *newObjectInfo = getNewObjectInfoInstance();
			switch (variableType)
			{
			case VARIABLETYPE_STRING:
				newObjectInfo->setStringValue(defaultValue->getStringValue());
				break;

			case VARIABLETYPE_INT:
				newObjectInfo->setIntValue(defaultValue->getIntValue());
				break;

			case VARIABLETYPE_REAL:
				newObjectInfo->setRealValue(defaultValue->getRealValue());
				break;

			default:
				Settings::error("Variable::setupDefaultInAnimation - invalid variable type");
				break;
			}

			TimelineKey *newTimelineKey = newVariableTimeline->pushBackKey(new TimeInfo(0, 0, new InstantEasingCurve()), newObjectInfo);
			newTimelineKey->setNextObjectInfo(newObjectInfo);
		}
	}

}
