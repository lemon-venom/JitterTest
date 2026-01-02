#include "..\headers\Timer.hpp"

Timer timer;

Timer::Timer()
{
	frameCounter_ = -1;
	fps_ = 0;
	fpsFrameCounter_ = 0;
	fpsTimeCounter_ = 0;
	timeDeltaMsec_ = 0.0;
	totalTime_ = 0.0;
}

Timer::~Timer()
{
}

void Timer::start()
{
	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond_);
	QueryPerformanceCounter((LARGE_INTEGER*)&ticksAtStartup_);
	started_ = true;
}

int32_t Timer::getFps()
{
	return fps_;
}

int32_t Timer::getFrameCount()
{
	return frameCounter_;
}

double Timer::getDeltaTime()
{
	return timeDelta_;
}

double Timer::getDeltaTimeMsec()
{
	return timeDeltaMsec_;
}

double Timer::getTotalTime()
{
	return totalTime_;
}

uint64_t Timer::frameTick()
{
	if (started_ == true)
	{
		// Save the time at the previous tick.
		timePreviousUsec_ = timeCurrentUsec_;

		timeCurrentUsec_ = tick();

		timeDeltaUsec_ = timeCurrentUsec_ - timePreviousUsec_;

		timeDeltaMsec_ = timeDeltaUsec_ / 1000L;

		timeDelta_ = (double)timeDeltaUsec_ / 1000000.0;

		frameCounter_++;

		totalTimeMsec_ += timeDeltaMsec_;

		totalTime_ += timeDelta_;
	}

	fpsTimeCounter_ += timeDelta_;

	fpsFrameCounter_++;

	// Count the number of frames displayed each second.
	bool setFps = false;

	while (fpsTimeCounter_ > 1.0)
	{
		fpsTimeCounter_ -= 1.0;

		setFps = true;
	}

	if (setFps == true)
	{
		fps_ = fpsFrameCounter_;

		//std::cout << "\rFPS: " << fps_ << std::flush;

		fpsFrameCounter_ = 0;
	}

	return timeCurrentUsec_;
}

uint64_t Timer::tick()
{
	uint64_t ticks;

	QueryPerformanceCounter((LARGE_INTEGER*)&ticks);

	// Remove startup ticks.
	ticks -= ticksAtStartup_;

	uint64_t seconds = ticks / ticksPerSecond_;

	uint64_t remainder = ticks % ticksPerSecond_;

	uint64_t timeUsec = (((double)remainder / (double)ticksPerSecond_) * 1000000L) + (seconds * 1000000L);

	return timeUsec;
}
