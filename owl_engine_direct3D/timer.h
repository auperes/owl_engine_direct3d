#pragma once

#include <chrono>

namespace owl
{
	class timer
	{
	public:
		timer();

		float mark();
		float peek() const;

	private:
		std::chrono::steady_clock::time_point last;
	};
}
