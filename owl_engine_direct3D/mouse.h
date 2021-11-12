#pragma once

#include <queue>

namespace owl
{
	class mouse
	{
		friend class window;

	public:
		class event
		{
		public:
			enum class type
			{
				LeftPressed,
				LeftReleased,
				RightPressed,
				RightReleased,
				WheelUp,
				WheelDown,
				Move,
				Enter,
				Leave,
				Invalid,
			};

			event() noexcept = default;
			event(type type, const mouse& parent) noexcept
				: type(type)
				, is_left_pressed(parent.is_left_pressed)
				, is_right_pressed(parent.is_right_pressed)
				, x(parent.x)
				, y(parent.y)
			{}

			bool is_valid() const noexcept { return type != type::Invalid; }
			type get_type() const noexcept { return type; }
			std::pair<int, int> get_position() const noexcept { return std::make_pair(x, y); }
			int get_position_x() const noexcept { return x; }
			int get_position_y() const noexcept { return y; }
			bool is_left_button_pressed() const noexcept { return is_left_pressed; }
			bool is_right_button_pressed() const noexcept { return is_right_pressed; }

		private:
			type type = type::Invalid;
			bool is_left_pressed = false;
			bool is_right_pressed = false;
			int x = 0;
			int y = 0;

		};

		mouse() = default;
		mouse(const mouse&) = delete;
		mouse& operator=(const mouse&) = delete;

		std::pair<int, int> get_position() const noexcept { return std::make_pair(x, y); }
		int get_position_x() const noexcept { return x; }
		int get_position_y() const noexcept { return y; }
		bool is_left_button_pressed() const noexcept { return is_left_pressed; }
		bool is_right_button_pressed() const noexcept { return is_right_pressed; }
		bool is_inside_window() const noexcept { return is_in_window; }
		bool is_empty() const noexcept { return event_buffer.empty(); }

		mouse::event read() noexcept;
		void flush() noexcept;

	private:
		static constexpr unsigned buffer_size = 16U;
		bool is_left_pressed = false;
		bool is_right_pressed = false;
		bool is_in_window = false;
		int x = 0;
		int y = 0;
		int wheel_delta_carry = 0;
		std::queue<mouse::event> event_buffer;

		void on_mouse_move(int x, int y) noexcept;
		void on_mouse_enter() noexcept;
		void on_mouse_leave() noexcept;
		void on_left_button_pressed(int x, int y) noexcept;
		void on_left_button_released(int x, int y) noexcept;
		void on_right_button_pressed(int x, int y) noexcept;
		void on_right_button_released(int x, int y) noexcept;
		void on_wheel_up(int x, int y) noexcept;
		void on_wheel_down(int x, int y) noexcept;
		void on_wheel_delta(int x, int y, int delta) noexcept;
		void trim_buffer() noexcept;
	};
}
