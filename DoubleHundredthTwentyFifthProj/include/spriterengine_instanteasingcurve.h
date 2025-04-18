#ifndef INSTANTEASINGCURVE_H
#define INSTANTEASINGCURVE_H

#include "spriterengine_easingcurveinterface.h"

namespace SpriterEngine
{

	class InstantEasingCurve : public EasingCurveInterface
	{
	public:
		InstantEasingCurve();

		real adjustedTimeRatio(real timeRatio) override;

		EasingCurveInterface *clone() override;
	};

}

#endif // INSTANTEASINGCURVE_H
