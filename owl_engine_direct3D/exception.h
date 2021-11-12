#pragma once

#include <exception>
#include <string>

namespace owl
{
	class exception : public std::exception
	{
	public:
		exception(int line, const char* file) noexcept;
		virtual ~exception() = default;

		const char* what() const noexcept override;
		virtual const char* get_type() const noexcept;
		int get_line() const noexcept;
		const std::string& get_file() const noexcept;
		std::string get_origin_string() const noexcept;

	protected:
		mutable std::string what_buffer;

	private:
		int line;
		std::string file;
	};
}
