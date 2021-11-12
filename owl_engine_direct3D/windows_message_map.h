#pragma once
#include <unordered_map>
#include <Windows.h>

class windows_message_map
{
public:
	windows_message_map();

	std::string operator()(DWORD message, LPARAM l_parameter, WPARAM w_parameter) const;

private:
	std::unordered_map<DWORD, std::string> message_map;
};
