#include <cassert>
#include <corgi/ShortString.h>
#include <cstring>

using namespace corgi;

/// <summary>
/// str should only have 15 characters
/// </summary>
/// <param name="str"></param>
ShortString::ShortString(const char* str)
{
	const auto size = strlen(str)+1;

	/*if(size>24)
	{
		assert(size < 24);
	}*/
	
	memcpy(_str, str, size);
}

const char* ShortString::c_str() const
{
	return &_str[0];
}

ShortString& ShortString::operator=(const char* str)
{
	const auto size = strlen(str) + 1;
	memcpy(_str, str, size);
	return *this;
}

bool ShortString::operator==(const ShortString& other) const noexcept
{
	return strcmp(_str, other._str) == 0;
}

bool ShortString::operator!=(const ShortString& other) const noexcept
{
	return strcmp(_str, other._str) != 0;
}

bool ShortString::operator<(const ShortString& other) const noexcept
{
	return strcmp(_str, other._str)<0;
}
