#include "windows_message_map.h"
#include <string>
#include <sstream>
#include <iomanip>

#define WM_UAHDESTROYWINDOW 0x0090
#define WM_UAHDRAWMENU 0x0091
#define WM_UAHDRAWMENUITEM 0x0092
#define WM_UAHINITMENU 0x0093
#define WM_UAHMEASUREMENUITEM 0x0094
#define WM_UAHNCPAINTMENUPOPUP 0x0095

#define REGISTER_MESSAGE(message) { message, #message }

windows_message_map::windows_message_map()
	: message_map(
		{
			REGISTER_MESSAGE(WM_CREATE),

			REGISTER_MESSAGE(WM_ACTIVATE),
			REGISTER_MESSAGE(WM_ACTIVATEAPP),

			//TODO
		}
		)
{}

std::string windows_message_map::operator()(DWORD message, LPARAM l_parameter, WPARAM w_parameter) const
{
	constexpr int first_column_width = 25;
	const auto i = message_map.find(message);

	std::ostringstream output_stream;

	if (i != message_map.end())
		output_stream << std::left << std::setw(first_column_width) << i->second << std::right;
	else
	{
		std::ostringstream bad_stream;
		bad_stream << "Unknown message: 0x" << std::hex << message;
		output_stream << std::left << std::setw(first_column_width) << bad_stream.str() << std::right;
	}

	output_stream << "    LP: 0x" << std::hex << std::setfill('0') << std::setw(8) << l_parameter;
	output_stream << "    WP: 0x" << std::hex << std::setfill('0') << std::setw(8) << w_parameter << std::endl;

	return output_stream.str();
}
