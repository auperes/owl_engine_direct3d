#include "graphics.h"

#include <sstream>

#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "dxerr.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GRAPHICS_THROW_FAILED(handle_result_call) if (FAILED(handle_result = handle_result_call)) throw owl::graphics::handle_result_exception(__LINE__, __FILE__, handle_result)
#define GRAPHICS_REMOVED_EXCEPTION(handle_result) owl::graphics::device_removed_exception(__LINE__, __FILE__, handle_result)

#define GRAPHICS_EXCEPTION_NOINFO(handle_result) owl::graphics::handle_result_exception(__LINE__, __FILE__, (handle_result))
#define GRAPHICS_THROW_NOINFO(handle_result_call) if (FAILED(handle_result = (handle_result_call))) throw owl::graphics::handle_result_exception(__LINE__, __FILE__, handle_result)

#ifndef NDEBUG
#define GRAPHICS_EXCEPTION(handle_result) owl::graphics::handle_result_exception(__LINE__, __FILE__, (handle_result), information_manager.GetMessages())
#define GRAPHICS_THROW_INFO(handle_result_call) information_manager.Set(); if (FAILED(handle_result = (handle_result_call))) throw GRAPHICS_EXCEPTION(handle_result)
#define GRAPHICS_DEVICE_REMOVED_EXCEPTION(handle_result) owl::graphics::device_removed_exception(__LINE__, __FILE__, (handle_result), information_manager.GetMessages())
#define GRAPHICS_THROW_INFO_ONLY(call) information_manager.Set(); (call); { auto v = information_manager.GetMessages(); if (!v.empty()) { throw owl::graphics::information_exception(__LINE__, __FILE__, v); } }
#else
#define GRAPHICS_EXCEPTION(handle_result) owl::graphics::handle_result_exception(__LINE__, __FILE__, (handle_result))
#define GRAPHICS_THROW_INFO(handle_result_call) GRAPHICS_THROW_NOINFO(handle_result_call)
#define GRAPHICS_DEVICE_REMOVED_EXCEPTION(handle_result) owl::graphics::device_removed_exception(__LINE__, __FILE__, (handle_result))
#define GRAPHICS_THROW_INFO_ONLY(call) (call)
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

		wrl::ComPtr<ID3D11Resource> back_buffer;
		GRAPHICS_THROW_INFO(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer));
		GRAPHICS_THROW_INFO(device->CreateRenderTargetView(back_buffer.Get(), nullptr, &render_target_view));
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
		context->ClearRenderTargetView(render_target_view.Get(), color);
	}

	void graphics::draw_triangle(float angle, float x, float y)
	{
		HRESULT handle_result;

		struct Vertex
		{
			struct {
				float x;
				float y;
			} position;
			struct {
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} color;
		};

		Vertex vertices[] =
		{
			{ 0.0F, 0.5F, 255, 0, 0, 0 },
			{ 0.5F, -0.5F, 0, 255, 0, 0 },
			{ -0.5F, -0.5F, 0, 0, 255, 0 },
			{ -0.3F, 0.3F, 0, 255, 0, 0 },
			{ 0.3F, 0.3F, 0, 0, 255, 0 },
			{ 0.0F, -1.0F, 255, 0, 0, 0 },
		};
		vertices[0].color.g = 255;

		wrl::ComPtr<ID3D11Buffer> vertex_buffer;
		D3D11_BUFFER_DESC vertex_buffer_descriptor = {};
		vertex_buffer_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertex_buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
		vertex_buffer_descriptor.CPUAccessFlags = 0U;
		vertex_buffer_descriptor.MiscFlags = 0U;
		vertex_buffer_descriptor.ByteWidth = sizeof(vertices);
		vertex_buffer_descriptor.StructureByteStride = sizeof(Vertex);

		D3D11_SUBRESOURCE_DATA vertex_subresource_data = {};
		vertex_subresource_data.pSysMem = vertices;

		GRAPHICS_THROW_INFO(device->CreateBuffer(&vertex_buffer_descriptor, &vertex_subresource_data, &vertex_buffer));

		const UINT stride = sizeof(Vertex);
		const UINT offset = 0U;

		context->IASetVertexBuffers(0U, 1U, vertex_buffer.GetAddressOf(), &stride, &offset);

		const unsigned short indices[] =
		{
			0, 1, 2,
			0, 2, 3,
			0, 4, 1,
			2, 1, 5
		};

		wrl::ComPtr<ID3D11Buffer> index_buffer;
		D3D11_BUFFER_DESC index_buffer_descriptor = {};
		index_buffer_descriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
		index_buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
		index_buffer_descriptor.CPUAccessFlags = 0U;
		index_buffer_descriptor.MiscFlags = 0U;
		index_buffer_descriptor.ByteWidth = sizeof(indices);
		index_buffer_descriptor.StructureByteStride = sizeof(unsigned short);

		D3D11_SUBRESOURCE_DATA index_subresource_data = {};
		index_subresource_data.pSysMem = indices;

		GRAPHICS_THROW_INFO(device->CreateBuffer(&index_buffer_descriptor, &index_subresource_data, &index_buffer));

		context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R16_UINT, 0U);

		struct constant_data
		{
			dx::XMMATRIX transformation;
		};

		const constant_data constant_data =
		{
			{
				dx::XMMatrixTranspose(
					dx::XMMatrixRotationZ(angle) *
					dx::XMMatrixScaling(3.0F / 4.0F, 1.0F, 1.0F) *
					dx::XMMatrixTranslation(x, y, 0.0F)
				)
			}
		};

		wrl::ComPtr<ID3D11Buffer> constant_buffer;
		D3D11_BUFFER_DESC constant_buffer_descriptor = {};
		constant_buffer_descriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constant_buffer_descriptor.Usage = D3D11_USAGE_DYNAMIC;
		constant_buffer_descriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constant_buffer_descriptor.MiscFlags = 0U;
		constant_buffer_descriptor.ByteWidth = sizeof(constant_data);
		constant_buffer_descriptor.StructureByteStride = 0U;

		D3D11_SUBRESOURCE_DATA constant_subresource_data = {};
		constant_subresource_data.pSysMem = &constant_data;

		GRAPHICS_THROW_INFO(device->CreateBuffer(&constant_buffer_descriptor, &constant_subresource_data, &constant_buffer));

		context->VSSetConstantBuffers(0U, 1U, constant_buffer.GetAddressOf());

		wrl::ComPtr<ID3D11PixelShader> pixel_shader;
		wrl::ComPtr<ID3DBlob> blob;
		GRAPHICS_THROW_INFO(D3DReadFileToBlob(L"../x64/Debug/pixel_passthrough.cso", &blob));
		GRAPHICS_THROW_INFO(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixel_shader));

		context->PSSetShader(pixel_shader.Get(), nullptr, 0U);

		wrl::ComPtr<ID3D11VertexShader> vertex_shader;
		GRAPHICS_THROW_INFO(D3DReadFileToBlob(L"../x64/Debug/vertex_passthrough.cso", &blob));
		GRAPHICS_THROW_INFO(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertex_shader));

		context->VSSetShader(vertex_shader.Get(), nullptr, 0U);

		wrl::ComPtr<ID3D11InputLayout> input_layout;
		const D3D11_INPUT_ELEMENT_DESC input_element_descriptors[] =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "Color", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 8U, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		GRAPHICS_THROW_INFO(device->CreateInputLayout(
			input_element_descriptors,
			static_cast<UINT>(std::size(input_element_descriptors)),
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			&input_layout));

		context->IASetInputLayout(input_layout.Get());
		context->OMSetRenderTargets(1U, render_target_view.GetAddressOf(), nullptr);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D11_VIEWPORT viewport = {};
		viewport.Width = 800;
		viewport.Height = 600;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		context->RSSetViewports(1U, &viewport);

		GRAPHICS_THROW_INFO_ONLY(context->DrawIndexed(static_cast<UINT>(std::size(indices)), 0U, 0U));
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

	graphics::information_exception::information_exception(int line, const char* file, std::vector<std::string> information_messages) noexcept
		: graphics_exception(line, file)
	{
		for (const auto& message : information_messages)
		{
			informations += message;
			informations.push_back('\n');
		}

		if (!informations.empty())
			informations.pop_back();
	}

	const char* graphics::information_exception::what() const noexcept
	{
		std::ostringstream output;
		output << get_type() << std::endl
			<< "\n[Error Info]\n" << get_information_error() << std::endl << std::endl;
		output << get_origin_string();
		what_buffer = output.str();
		return what_buffer.c_str();
	}

	const char* graphics::information_exception::get_type() const noexcept
	{
		return "owl graphics information exception";
	}

	std::string graphics::information_exception::get_information_error() const noexcept
	{
		return informations;
	}
}
