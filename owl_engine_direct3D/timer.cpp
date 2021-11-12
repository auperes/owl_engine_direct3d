#include "timer.h"

namespace owl
{
	timer::timer()
	{
		last = std::chrono::steady_clock::now();
	}

	float timer::mark()
	{
		const auto old = last;
		last = std::chrono::steady_clock::now();
		const std::chrono::duration<float> frame_time = last - old;

		return frame_time.count();
	}

	float timer::peek() const
	{
		return std::chrono::duration<float>(std::chrono::steady_clock::now() - last).count();
	}
}
