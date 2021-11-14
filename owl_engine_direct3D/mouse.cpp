#include "mouse.h"

#include <Windows.h>

namespace owl
{
	std::optional<mouse::event> mouse::read() noexcept
	{
		if (event_buffer.size() > 0U)
		{
			mouse::event e = event_buffer.front();
			event_buffer.pop();
			return e;
		}
		
		return {};
	}

	void mouse::flush() noexcept
	{
		event_buffer = std::queue<mouse::event>();
	}

	void mouse::on_mouse_move(int new_x, int new_y) noexcept
	{
		x = new_x;
		y = new_y;
		event_buffer.push(mouse::event(mouse::event::type::Move, *this));
		trim_buffer();
	}

	void mouse::on_mouse_enter() noexcept
	{
		is_in_window = true;
		event_buffer.push(mouse::event(mouse::event::type::Enter, *this));
		trim_buffer();
	}

	void mouse::on_mouse_leave() noexcept
	{
		is_in_window = false;
		event_buffer.push(mouse::event(mouse::event::type::Leave, *this));
		trim_buffer();
	}

	void mouse::on_left_button_pressed(int x, int y) noexcept
	{
		is_left_pressed = true;
		event_buffer.push(mouse::event(mouse::event::type::LeftPressed, *this));
		trim_buffer();
	}

	void mouse::on_left_button_released(int x, int y) noexcept
	{
		is_left_pressed = false;
		event_buffer.push(mouse::event(mouse::event::type::LeftReleased, *this));
		trim_buffer();
	}

	void mouse::on_right_button_pressed(int x, int y) noexcept
	{
		is_right_pressed = true;
		event_buffer.push(mouse::event(mouse::event::type::RightPressed, *this));
		trim_buffer();
	}

	void mouse::on_right_button_released(int x, int y) noexcept
	{
		is_right_pressed = true;
		event_buffer.push(mouse::event(mouse::event::type::RightReleased, *this));
		trim_buffer();
	}

	void mouse::on_wheel_up(int x, int y) noexcept
	{
		event_buffer.push(mouse::event(mouse::event::type::WheelUp, *this));
		trim_buffer();
	}

	void mouse::on_wheel_down(int x, int y) noexcept
	{
		event_buffer.push(mouse::event(mouse::event::type::WheelDown, *this));
		trim_buffer();
	}

	void mouse::on_wheel_delta(int x, int y, int delta) noexcept
	{
		wheel_delta_carry += delta;

		// generate events for every 120
		while (wheel_delta_carry >= WHEEL_DELTA)
		{
			wheel_delta_carry -= WHEEL_DELTA;
			on_wheel_up(x, y);
		}
		while (wheel_delta_carry <= -WHEEL_DELTA)
		{
			wheel_delta_carry += WHEEL_DELTA;
			on_wheel_down(x, y);
		}
	}

	void mouse::trim_buffer() noexcept
	{
		while (event_buffer.size() > buffer_size)
		{
			event_buffer.pop();
		}
	}
}
