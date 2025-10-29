#pragma once

#include "Windows.h"
#include "Logger.hpp"

class Timer
{
public:

	Timer();
	virtual ~Timer();

	uint64_t	frameTick();		
	void		start();
	uint64_t	tick();

	int32_t	getFps();
	int32_t	getFrameCount();
	double	getDeltaTimeMsec();
	double	getDeltaTime();
	double	getTotalTime();

	const double fixedTimeStep = 1.0 / 100.0;

	double timeAccumulator = 0.0;

private:
	uint32_t	fps_;
	uint64_t	fpsTimeCounterMsec_;
	uint32_t	fpsFrameCounter_;
	uint64_t	frameCounter_;
	bool		started_;
	uint64_t	ticksPerSecond_;
	uint64_t	ticksAtStartup_;
	uint64_t	timeCurrentUsec_;
	double		timeDelta_;
	uint64_t	timeDeltaMsec_;
	uint64_t	timeDeltaUsec_;
	uint64_t	timePreviousUsec_;
	uint64_t	totalTimeMsec_;
	double		totalTime_;
};

extern Timer timer;
