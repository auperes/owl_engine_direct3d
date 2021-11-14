#pragma once

#include <bitset>
#include <optional>
#include <queue>

namespace owl
{
	class keyboard
	{
		friend class window;

	public:
		class event
		{
		public:
			enum class type
			{
				Press,
				Release
			};

			event(type type, unsigned char code) noexcept : type(type), code(code) {}

			bool is_pressed() const noexcept { return type == type::Press; }
			bool is_released() const noexcept { return type == type::Release; }
			unsigned char get_code() const noexcept { return code; }

		private:
			type type;
			unsigned char code;
		};

		keyboard() = default;
		keyboard(const keyboard&) = delete;

		keyboard& operator=(const keyboard&) = delete;

		bool is_key_pressed(unsigned char key_code) const noexcept;
		std::optional<event> read_key() noexcept;
		bool is_key_empty() const noexcept;
		void flush_key() noexcept;

		std::optional<char> read_char() noexcept;
		bool is_char_empty() const noexcept;
		void flush_char() noexcept;

		void flush() noexcept;

		void enable_auto_repeat() noexcept;
		void disable_auto_repeat() noexcept;
		bool is_auto_repeat_enabled() noexcept;

	private:
		void on_key_pressed(unsigned char key_code) noexcept;
		void on_key_released(unsigned char key_code) noexcept;
		void on_char(char character) noexcept;
		void clear_state() noexcept;

		template <typename T>
		static void trim_buffer(std::queue<T>& buffer) noexcept;

		static constexpr unsigned int keys_number = 256U;
		static constexpr unsigned int buffer_size = 16U;

		bool auto_repeat_enabled = false;

		std::bitset<keys_number> key_states;
		std::queue<owl::keyboard::event> key_buffer;
		std::queue<char> char_buffer;
	};

	template <typename T>
	void keyboard::trim_buffer(std::queue<T>& buffer) noexcept
	{
		while (buffer.size() > buffer_size)
			buffer.pop();
	}
}
