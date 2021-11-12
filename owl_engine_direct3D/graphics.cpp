#include "graphics.h"

#include <sstream>

#include "dxerr.h"

#pragma comment(lib, "d3d11.lib")

#define GRAPHICS_THROW_FAILED(handle_result_call) if (FAILED(handle_result = handle_result_call)) throw owl::graphics::handle_result_exception(__LINE__, __FILE__, handle_result)
#define GRAPHICS_REMOVED_EXCEPTION(handle_result) owl::graphics::device_removed_exception(__LINE__, __FILE__, handle_result)

#define GRAPHICS_EXCEPTION_NOINFO(handle_result) owl::graphics::handle_result_exception(__LINE__, __FILE__, (handle_result))
#define GRAPHICS_THROW_NOINFO(handle_result_call) if (FAILED(handle_result = (handle_result_call))) throw owl::graphics::handle_result_exception(__LINE__, __FILE__, handle_result)

#ifndef NDEBUG
#define GRAPHICS_EXCEPTION(handle_result) owl::graphics::handle_result_exception(__LINE__, __FILE__, (handle_result), information_manager.GetMessages())
#define GRAPHICS_THROW_INFO(handle_result_call) information_manager.Set(); if (FAILED(handle_result = (handle_result_call))) throw GRAPHICS_EXCEPTION(handle_result)
#define GRAPHICS_DEVICE_REMOVED_EXCEPTION(handle_result) owl::graphics::device_removed_exception(__LINE__, __FILE__, (handle_result), information_manager.GetMessages())
#else
#define GRAPHICS_EXCEPTION(handle_result) owl::graphics::handle_result_exception(__LINE__, __FILE__, (handle_result))
#define GRAPHICS_THROW_INFO(handle_result_call) GRAPHICS_THROW_NOINFO(handle_result_call)
#define GRAPHICS_DEVICE_REMOVED_EXCEPTION(handle_result) owl::graphics::device_removed_exception(__LINE__, __FILE__, (handle_result))
#endif

namespace owl
{
	graphics::graphics(HWND handle_window)
	{
		DXGI_SWAP_CHAIN_DESC swapchain_descriptor = {};
		swapchain_descriptor.BufferDesc.Width = 0;
		swapchain_descriptor.BufferDesc.Height = 0;
		swapchain_descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapchain_descriptor.BufferDesc.RefreshRate.Numerator = 0;
		swapchain_descriptor.BufferDesc.RefreshRate.Denominator = 0;
		swapchain_descriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapchain_descriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapchain_descriptor.SampleDesc.Count = 1;
		swapchain_descriptor.SampleDesc.Quality = 0;
		swapchain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_descriptor.BufferCount = 1;
		swapchain_descriptor.OutputWindow = handle_window;
		swapchain_descriptor.Windowed = TRUE;
		swapchain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapchain_descriptor.Flags = 0;

		UINT swap_create_flags = 0U;

#ifndef NDEBUG
		swap_create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT handle_result;

		GRAPHICS_THROW_INFO(D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swap_create_flags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapchain_descriptor,
			&swapchain,
			&device,
			nullptr,
			&context
		));

		ID3D11Resource* back_buffer = nullptr;
		GRAPHICS_THROW_INFO(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer)));
		GRAPHICS_THROW_INFO(device->CreateRenderTargetView(back_buffer, nullptr, &target_view));

		back_buffer->Release();
	}

	graphics::~graphics()
	{
		if (target_view != nullptr)
			target_view->Release();

		if (context != nullptr)
			device->Release();

		if (swapchain != nullptr)
			swapchain->Release();

		if (device != nullptr)
			device->Release();
	}

	void graphics::end_frame()
	{
		HRESULT handle_result;

#ifndef NDEBUG
		information_manager.Set();
#endif();

		if (FAILED(handle_result = swapchain->Present(1U, 0U)))
		{
			if (handle_result == DXGI_ERROR_DEVICE_REMOVED)
				throw GRAPHICS_DEVICE_REMOVED_EXCEPTION(device->GetDeviceRemovedReason());
			else
				throw GRAPHICS_EXCEPTION(handle_result);
		}
	}

	void graphics::clear_buffer(float red, float green, float blue) noexcept
	{
		const float color[] = { red, green, blue, 1.0F };
		context->ClearRenderTargetView(target_view, color);
	}

	graphics::handle_result_exception::handle_result_exception(int line, const char* file, HRESULT handle_result, std::vector<std::string> information_messages) noexcept
		: graphics_exception(line, file)
		, handle_result(handle_result)
	{
		for (const auto& message : information_messages)
		{
			informations += message;
			informations.push_back('\n');
		}

		if (!informations.empty())
			informations.pop_back();
	}

	const char* graphics::handle_result_exception::what() const noexcept
	{
		std::ostringstream stream;
		stream << get_type() << std::endl
			<< "[Error Code] 0x" << std::hex << std::uppercase << get_error_code()
			<< std::dec << " (" << static_cast<unsigned long>(get_error_code()) << ")" << std::endl
			<< "[Error String] " << get_error_string() << std::endl
			<< "[Description] " << get_error_description() << std::endl;
		if (!informations.empty())
		{
			stream << "\n[Error Info]\n" << get_error_information() << std::endl << std::endl;
		}
		stream << get_origin_string();
		what_buffer = stream.str();
		return what_buffer.c_str();
	}

	const char* graphics::handle_result_exception::get_type() const noexcept
	{
		return "owl graphics handle result exception";
	}

	HRESULT graphics::handle_result_exception::get_error_code() const noexcept
	{
		return handle_result;
	}
	std::string graphics::handle_result_exception::get_error_string() const noexcept
	{
		return DXGetErrorString(handle_result);
	}

	std::string graphics::handle_result_exception::get_error_description() const noexcept
	{
		char buffer[512];
		DXGetErrorDescription(handle_result, buffer, sizeof(buffer));
		return buffer;
	}

	std::string graphics::handle_result_exception::get_error_information() const noexcept
	{
		return informations;
	}

	const char* graphics::device_removed_exception::get_type() const noexcept
	{
		return "owl graphics exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
	}
}
