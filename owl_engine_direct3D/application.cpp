#include "application.h"

namespace owl
{
	application::application()
		: window(800, 600, "Owl Engine")
	{
	}

	int application::start()
	{
		MSG message;
		BOOL result;

		while (true)
		{
			if (const auto code = window::process_messages())
				return *code;

			do_frame();
		}

		if (result == -1)
			throw WINDOW_LAST_EXCEPTION();

		return message.wParam;
	}

	void application::do_frame()
	{
		const float color = ::sin(timer.peek()) / 2.0F + 0.5F;
		window.get_graphics().clear_buffer(color, color, 1.0F);
		window.get_graphics().draw_triangle(timer.peek());
		window.get_graphics().end_frame();
	}
}
