#ifndef ANGLEINFO_H
#define ANGLEINFO_H

#include "spriterengine_global.h"

namespace SpriterEngine
{

	class AngleInfo
	{
	public:
		AngleInfo();

		real angleLinear(real angleB, real t);

		real angle;
		int spinDirection;
	};

}

#endif // ANGLEINFO_H
