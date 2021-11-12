#include "exception.h"

#include <sstream>

namespace owl
{
	exception::exception(int line, const char* file) noexcept
		: line(line)
		, file(file)
	{
	}

	const char* exception::what() const noexcept
	{
		std::ostringstream stream;
		stream << get_type() << std::endl
			<< get_origin_string();
		what_buffer = stream.str();

		return what_buffer.c_str();
	}

	const char* exception::get_type() const noexcept
	{
		return "owl exception";
	}

	int exception::get_line() const noexcept
	{
		return line;
	}

	const std::string& exception::get_file() const noexcept
	{
		return file;
	}

	std::string exception::get_origin_string() const noexcept
	{
		std::ostringstream stream;
		stream << "[File] " << file << std::endl
			<< "[Line] " << line;

		return stream.str();
	}
}
