#include "keyboard.h"

namespace owl
{
	bool keyboard::is_key_pressed(unsigned char key_code) const noexcept
	{
		return key_states[key_code];
	}

	std::optional<keyboard::event> keyboard::read_key() noexcept
	{
		if (key_buffer.size() > 0U)
		{
			keyboard::event e = key_buffer.front();
			key_buffer.pop();
			return e;
		}
		
		return {};
	}

	bool keyboard::is_key_empty() const noexcept
	{
		return key_buffer.empty();
	}

	void keyboard::flush_key() noexcept
	{
		key_buffer = std::queue<keyboard::event>();
	}

	std::optional<char> keyboard::read_char() noexcept
	{
		if (char_buffer.size() > 0U)
		{
			unsigned char char_code = char_buffer.front();
			char_buffer.pop();
			return char_code;
		}
		
		return {};
	}

	bool keyboard::is_char_empty() const noexcept
	{
		return char_buffer.empty();
	}

	void keyboard::flush_char() noexcept
	{
		char_buffer = std::queue<char>();
	}

	void keyboard::flush() noexcept
	{
		flush_key();
		flush_char();
	}

	void keyboard::enable_auto_repeat() noexcept
	{
		auto_repeat_enabled = true;
	}

	void keyboard::disable_auto_repeat() noexcept
	{
		auto_repeat_enabled = false;
	}

	bool keyboard::is_auto_repeat_enabled() noexcept
	{
		return auto_repeat_enabled;
	}

	void keyboard::on_key_pressed(unsigned char key_code) noexcept
	{
		key_states[key_code] = true;
		key_buffer.push(keyboard::event(keyboard::event::type::Press, key_code));
		trim_buffer(key_buffer);
	}

	void keyboard::on_key_released(unsigned char key_code) noexcept
	{
		key_states[key_code] = false;
		key_buffer.push(keyboard::event(keyboard::event::type::Release, key_code));
		trim_buffer(key_buffer);
	}

	void keyboard::on_char(char character) noexcept
	{
		char_buffer.push(character);
		trim_buffer(char_buffer);
	}

	void keyboard::clear_state() noexcept
	{
		key_states.reset();
	}
}
