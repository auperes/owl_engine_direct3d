#pragma once

#include "owl_window.h"
#include "timer.h"

namespace owl
{
	class application
	{
	public:
		application();

		int start();
	private:
		void do_frame();

		window window;
		timer timer;
	};
}
