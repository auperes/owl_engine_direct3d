#pragma once

#include <memory>
#include <optional>

#include "exception.h"
#include "graphics.h"
#include "keyboard.h"
#include "lean_window.h"
#include "mouse.h"

namespace owl
{
	class window
	{
	public:
		class window_exception : public exception
		{
			using exception::exception;

		public:
			~window_exception() override = default;

			static std::string translate_error_code(HRESULT handle_result);
		};

		class window_handle_result_exception : public window_exception
		{
		public:
			window_handle_result_exception(int line, const char* file, HRESULT handle_result) noexcept;

			const char* what() const noexcept override;
			const char* get_type() const noexcept override;
			HRESULT get_error_code() const noexcept;
			std::string get_error_description() const noexcept;

		private:
			HRESULT handle_result;
		};

		class no_graphics_exception : public exception
		{
		public:
			using exception::exception;
			const char* get_type() const noexcept override;
		};

		window(int width, int height, const char* name);
		window(const window&) = delete;

		~window();

		window& operator=(const window&) = delete;

		static std::optional<int> process_messages() noexcept;

		void set_title(const std::string& title);
		graphics& get_graphics();

		keyboard keyboard;
		mouse mouse;

	private:
		class window_class
		{
		public:
			static const char* get_name() noexcept;
			static HINSTANCE get_instance() noexcept;

		private:
			window_class() noexcept;
			window_class(const window_class&) = delete;

			~window_class();

			window_class& operator=(const window_class&) = delete;

			static constexpr const char* window_class_name = "Owl Engine Direct3D Window";
			static window_class wnd_class;
			HINSTANCE handle_instance;
		};

		static LRESULT CALLBACK handle_message_setup(HWND handle_window, UINT message, WPARAM w_parameter, LPARAM l_parameter) noexcept;
		static LRESULT CALLBACK handle_message_thunk(HWND handle_window, UINT message, WPARAM w_parameter, LPARAM l_parameter) noexcept;
		LRESULT handle_message(HWND handle_window, UINT message, WPARAM w_parameter, LPARAM l_parameter) noexcept;

		int width;
		int height;
		HWND handle_window;
		std::unique_ptr<graphics> _graphics;
	};
}

#define WINDOW_EXCEPTION(handle_result) owl::window::window_handle_result_exception(__LINE__, __FILE__, (handle_result))
#define WINDOW_LAST_EXCEPTION() owl::window::window_handle_result_exception(__LINE__, __FILE__, GetLastError())
#define WINDOW_NO_GRAPHICS_EXCEPTION() owl::window::no_graphics_exception(__LINE__, __FILE__)