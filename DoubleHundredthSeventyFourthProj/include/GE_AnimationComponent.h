#pragma once

#include "SDL.h"

struct AnimationComponent
{
	int m_numFrames;
	int m_currentFrame;
	int m_frameSpeedRate;
	bool m_shouldLoop;
	unsigned int m_startTime;

	AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool shouldLoop = true)
		: m_numFrames(numFrames)
		, m_currentFrame(1)
		, m_frameSpeedRate(frameSpeedRate)
		, m_shouldLoop(shouldLoop)
		, m_startTime(SDL_GetTicks())
	{
	}
};