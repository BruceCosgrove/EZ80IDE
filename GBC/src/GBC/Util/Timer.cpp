#include "gbcpch.h"
#include "Timer.h"

namespace gbc
{
	Timer::Timer()
	{
		Reset();
	}

	void Timer::Reset()
	{
		start = std::chrono::high_resolution_clock::now();
	}

	Timestep Timer::GetElapsedTime() const
	{
		auto end = std::chrono::high_resolution_clock::now();
		return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000000000.0);
	}
}
