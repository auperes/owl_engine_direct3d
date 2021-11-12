#pragma once

#include <string>
#include <vector>

#include "exception.h"
#include "lean_window.h"
#include <d3d11.h>

#include "DxgiInfoManager.h"

namespace owl
{
	class graphics
	{
	public:
		class graphics_exception : public exception
		{
			using exception::exception;
		};

		class handle_result_exception : public graphics_exception
		{
		public:
			handle_result_exception(int line, const char* file, HRESULT handle_result, std::vector<std::string> info_messages = {}) noexcept;

			const char* what() const noexcept override;
			const char* get_type() const noexcept override;
			HRESULT get_error_code() const noexcept;
			std::string get_error_string() const noexcept;
			std::string get_error_description() const noexcept;
			std::string get_error_information() const noexcept;

		private:
			HRESULT handle_result;
			std::string informations;
		};

		class device_removed_exception : public handle_result_exception
		{
			using handle_result_exception::handle_result_exception;

		public:
			const char* get_type() const noexcept override;

		private:
			std::string reason;
		};

		graphics(HWND handle_window);
		graphics(const graphics&) = delete;

		~graphics();

		graphics& operator=(const graphics&) = delete;

		void end_frame();
		void clear_buffer(float red, float green, float blue) noexcept;

	private:
#ifndef NDEBUG
		DxgiInfoManager information_manager;
#endif
		ID3D11Device* device = nullptr;
		IDXGISwapChain* swapchain = nullptr;
		ID3D11DeviceContext* context = nullptr;
		ID3D11RenderTargetView* target_view = nullptr;
	};
}

