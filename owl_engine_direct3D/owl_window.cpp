#include "owl_window.h"

#include <sstream>

#include "resource.h"

namespace owl
{
	window::window_class window::window_class::wnd_class;

	window::window_class::window_class() noexcept
		: handle_instance(GetModuleHandle(nullptr))
	{
		WNDCLASSEX window_class = { 0 };
		window_class.cbSize = sizeof(window_class);
		window_class.style = CS_OWNDC;
		window_class.lpfnWndProc = handle_message_setup;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = get_instance();
		window_class.hIcon = static_cast<HICON>(LoadImage(handle_instance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
		window_class.hCursor = nullptr;
		window_class.hbrBackground = nullptr;
		window_class.lpszMenuName = nullptr;
		window_class.lpszClassName = get_name();
		window_class.hIconSm = static_cast<HICON>(LoadImage(handle_instance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));

		RegisterClassEx(&window_class);
	}

	window::window_class::~window_class()
	{
		UnregisterClass(window_class_name, get_instance());
	}

	const char* window::window_class::get_name() noexcept
	{
		return window_class_name;
	}

	HINSTANCE window::window_class::get_instance() noexcept
	{
		return wnd_class.handle_instance;
	}

	window::window(int width, int height, const char* name)
		: width(width)
		, height(height)
	{
		RECT window_rectangle = {};
		window_rectangle.left = 100;
		window_rectangle.right = width + window_rectangle.left;
		window_rectangle.top = 100;
		window_rectangle.bottom = height + window_rectangle.top;

		if (AdjustWindowRect(&window_rectangle, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
			throw WINDOW_LAST_EXCEPTION();

		handle_window = CreateWindow(
			window_class::get_name(), name,
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT, window_rectangle.right - window_rectangle.left, window_rectangle.bottom - window_rectangle.top,
			nullptr, nullptr, window_class::get_instance(), this);

		if (handle_window == nullptr)
			throw WINDOW_LAST_EXCEPTION();

		ShowWindow(handle_window, SW_SHOWDEFAULT);

		_graphics = std::make_unique<graphics>(handle_window);
	}

	window::~window()
	{
		DestroyWindow(handle_window);
	}

	void window::set_title(const std::string& title)
	{
		if (SetWindowText(handle_window, title.c_str()) == 0)
			throw WINDOW_LAST_EXCEPTION();
	}

	graphics& window::get_graphics()
	{
		if (!_graphics)
			throw WINDOW_NO_GRAPHICS_EXCEPTION();

		return *_graphics;
	}


	LRESULT CALLBACK window::handle_message_setup(HWND handle_window, UINT message, WPARAM w_parameter, LPARAM l_parameter) noexcept
	{
		if (message == WM_NCCREATE)
		{
			const CREATESTRUCTW* const create = reinterpret_cast<CREATESTRUCTW*>(l_parameter);
			window* const window = static_cast<owl::window*>(create->lpCreateParams);
			SetWindowLongPtr(handle_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			SetWindowLongPtr(handle_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window::handle_message_thunk));

			return window->handle_message(handle_window, message, w_parameter, l_parameter);
		}

		return DefWindowProc(handle_window, message, w_parameter, l_parameter);
	}

	LRESULT CALLBACK window::handle_message_thunk(HWND handle_window, UINT message, WPARAM w_parameter, LPARAM l_parameter) noexcept
	{
		window* const window = reinterpret_cast<owl::window*>(GetWindowLongPtr(handle_window, GWLP_USERDATA));
		return window->handle_message(handle_window, message, w_parameter, l_parameter);
	}

	LRESULT window::handle_message(HWND handle_window, UINT message, WPARAM w_parameter, LPARAM l_parameter) noexcept
	{
		switch (message)
		{
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
		case WM_KILLFOCUS:
			keyboard.clear_state();
			break;

			/* keyboard messages begin */
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (!(l_parameter & 0x40000000) || keyboard.is_auto_repeat_enabled())
				keyboard.on_key_pressed(static_cast<unsigned char>(w_parameter));
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			keyboard.on_key_released(static_cast<unsigned char>(w_parameter));
			break;
		case WM_CHAR:
			keyboard.on_char(static_cast<unsigned char>(w_parameter));
			break;
			/* keyboard messages end */

			/* mouse messages begin */
		case WM_MOUSEMOVE:
		{
			const POINTS point = MAKEPOINTS(l_parameter);

			// in client region
			if (point.x >= 0 && point.x < width && point.y >= 0 && point.y < height)
			{
				mouse.on_mouse_move(point.x, point.y);
				if (!mouse.is_inside_window())
				{
					SetCapture(handle_window);
					mouse.on_mouse_enter();
				}
			}
			// not in client region
			else
			{
				if (w_parameter & (MK_LBUTTON | MK_RBUTTON)) // button down
					mouse.on_mouse_move(point.x, point.y);
				else // button up
				{
					ReleaseCapture();
					mouse.on_mouse_leave();
				}
			}
			break;
		}
		case WM_LBUTTONDOWN:
		{
			const POINTS point = MAKEPOINTS(l_parameter);
			mouse.on_left_button_pressed(point.x, point.y);
			break;
		}
		case WM_LBUTTONUP:
		{
			const POINTS point = MAKEPOINTS(l_parameter);
			mouse.on_left_button_released(point.x, point.y);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			const POINTS point = MAKEPOINTS(l_parameter);
			mouse.on_right_button_pressed(point.x, point.y);
			break;
		}
		case WM_RBUTTONUP:
		{
			const POINTS point = MAKEPOINTS(l_parameter);
			mouse.on_right_button_released(point.x, point.y);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			const POINTS point = MAKEPOINTS(l_parameter);
			const int delta = GET_WHEEL_DELTA_WPARAM(w_parameter);
			mouse.on_wheel_delta(point.x, point.y, delta);
			break;
		}
		/* mouse messages end */
		}

		return DefWindowProc(handle_window, message, w_parameter, l_parameter);
	}

	std::optional<int> window::process_messages() noexcept
	{
		MSG message;

		while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				return message.wParam;

			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return {};
	}

	std::string window::window_exception::translate_error_code(HRESULT handle_result)
	{
		char* message_buffer = nullptr;
		DWORD message_length = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, handle_result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&message_buffer), 0, nullptr);

		if (message_length == 0)
			return "Unidentified error code";

		std::string error_string = message_buffer;
		LocalFree(message_buffer);
		return error_string;
	}

	window::window_handle_result_exception::window_handle_result_exception(int line, const char* file, HRESULT handle_result) noexcept
		: window_exception(line, file)
		, handle_result(handle_result)
	{
	}

	const char* window::window_handle_result_exception::window_handle_result_exception::what() const noexcept
	{
		std::ostringstream output;
		output << get_type() << std::endl
			<< "[Error Code] 0x" << std::hex << std::uppercase << get_error_code()
			<< std::dec << " (" << static_cast<unsigned long>(get_error_code()) << ")" << std::endl
			<< "[Description] " << get_error_description() << std::endl
			<< get_origin_string();
		what_buffer = output.str();
		return what_buffer.c_str();
	}

	const char* window::window_handle_result_exception::get_type() const noexcept
	{
		return "owl window handle result exception";
	}

	HRESULT window::window_handle_result_exception::get_error_code() const noexcept
	{
		return handle_result;
	}

	std::string window::window_handle_result_exception::get_error_description() const noexcept
	{
		return translate_error_code(handle_result);
	}

	const char* window::no_graphics_exception::get_type() const noexcept
	{
		return "owl window exception [No Graphics]";
	}
}