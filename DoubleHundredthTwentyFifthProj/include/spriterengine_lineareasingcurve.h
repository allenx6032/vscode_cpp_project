#ifndef LINEAREASINGCURVE_H
#define LINEAREASINGCURVE_H

#include "spriterengine_easingcurveinterface.h"

namespace SpriterEngine
{

	class LinearEasingCurve : public EasingCurveInterface
	{
	public:
		LinearEasingCurve();

		real adjustedTimeRatio(real timeRatio) override;

		EasingCurveInterface *clone() override;
	};

}

#endif // LINEAREASINGCURVE_H
