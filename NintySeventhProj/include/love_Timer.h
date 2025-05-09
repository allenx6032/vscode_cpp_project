/**
 * Copyright (c) 2006-2015 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#ifndef LOVE_TIMER_TIMER_H
#define LOVE_TIMER_TIMER_H

// LOVE
#include "love_Module.h"

namespace love
{
namespace timer
{

class Timer : public Module
{
public:

	virtual ~Timer() {}

	// Implements Module.
	virtual ModuleType getModuleType() const { return M_TIMER; }

	/**
	 * Measures the time between this call and the previous call,
	 * and updates internal values accordinly.
	 **/
	virtual void step() = 0;

	/**
	 * Tries to sleep for the specified amount of time. The precision is
	 * usually 1ms.
	 * @param seconds The number of seconds to sleep for.
	 **/
	virtual void sleep(double seconds) const = 0;

	/**
	 * Gets the time between the last two frames, assuming step is called
	 * each frame.
	 **/
	virtual double getDelta() const = 0;

	/**
	 * Gets the average FPS over the last second. Beucase the value is only updated
	 * once per second, it does not look erratic when displayed on screen.
	 * @return The "current" FPS.
	 **/
	virtual int getFPS() const = 0;

	/**
	 * Gets the average delta time (seconds per frame) over the last second.
	 **/
	virtual double getAverageDelta() const = 0;

	/**
	 * Gets the amount of time passed since an unspecified time. Useful for
	 * profiling code or measuring intervals. The time is microsecond-precise,
	 * and increases monotonically.
	 * @return The time (in seconds)
	 **/
	virtual double getTime() const = 0;

}; // Timer

} // timer
} // love

#endif // LOVE_TIMER_TIMER_H
