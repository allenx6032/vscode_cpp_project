#include "SLO_Interpolator.h"
#define _USE_MATH_DEFINES
#include <cmath>

std::function<double(double)> InterpolatorFactory::create(InterpolatorType type)
{
	switch(type) {
	case InterpolatorType::AccelerateDecelerate:{
		return [](double input)->double{
			return (float)(std::cos((input + 1.0) * M_PI) / 2.0) + 0.5;
		};
	}
	case InterpolatorType::Accelerate:{
		const double factor2_ = 2.0;
		return [factor2_](double input)->double{
			if (factor2_ == 2.0f) {
				return input * input;
			}
			else {
				return std::pow(input, factor2_);
			}
		};
	}
	case InterpolatorType::Anticipate:{
		const double tension_ = 1.0;
		return [tension_](double input)->double{
			return input * input * ((tension_ + 1) * input - tension_);
		};
	}
	case InterpolatorType::AnticipateOvershoot:{
		const double tension_ = 1.5;
		return [tension_](double input)->double{
			if (input < 0.5f) {
				auto a = [](double t, double s) -> double {
					return t * t * ((s + 1) * t - s);
				};
				return 0.5f * a(input * 2.0f, tension_);
			}
			else {
				auto o = [](double t, double s) -> double {
					return t * t * ((s + 1) * t + s);
				};
				return 0.5f * (o(input * 2.0f - 2.0f, tension_) + 2.0f);
			}
		};
	}
	case InterpolatorType::Bounce:{
		return [](double input)->double{
			auto bounce = [](double t) -> double {
		        return t * t * 8.0;
		    };

			input *= 1.1226;
		    if (input < 0.3535) return bounce(input);
		    else if (input < 0.7408) return bounce(input - 0.54719) + 0.7;
		    else if (input < 0.9644) return bounce(input - 0.8526) + 0.9;
		    else return bounce(input - 1.0435) + 0.95;
		};
	}
	case InterpolatorType::Cycle:{
		const double cycles_ = 2.0;
		return [cycles_](double input)->double{
			return (std::sin(2 * cycles_ * M_PI * input));
		};
	}
	case InterpolatorType::Decelerate:{
		const double factor_ = 2.0;
		return [factor_](double input)->double{
		    if (factor_ == 1.0) {
		        return (1.0 - (1.0 - input) * (1.0 - input));
		    } else {
		        return (1.0 - std::pow((1.0 - input), 2 * factor_));
		    }
		};
	}
	case InterpolatorType::Overshoot:{
		const double tension_ = 1.5;
		return [tension_](double input)->double{
		    input -= 1.0;
		    return input * input * ((tension_ + 1.0) * input + tension_) + 1.0;
		};
	}
	case InterpolatorType::Linear:
	default:
		return [](double input)->double{
			return input;
		};
	}
}

