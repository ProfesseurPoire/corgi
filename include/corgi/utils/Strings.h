#pragma once

#include <string>

namespace corgi
{
	static char* convert_std_string(const std::string& str)
	{
		auto cstr = new char[str.size() + 1];
		str.copy(cstr, str.size());
		cstr[str.size()] = '\0';
		return cstr;
	}
}