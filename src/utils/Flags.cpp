#include <corgi/utils/Flags.h>
#include <stdexcept>

namespace corgi
{

	Flags::Flags(char c) : _flags(c)
	{}

	bool Flags::operator==(const Flags& other) const noexcept
	{
		return _flags == other._flags;
	}

	bool Flags::operator!=(const Flags& other) const noexcept
	{
		return !(*this == other);
	}

	bool Flags::operator<(const Flags& other) const noexcept
	{
		return _flags < other._flags;
	}

	void Flags::set_bit(char position, bool value)
	{
		check_argument(position);
		
		if (value)
			_flags |= 1u << position;
		else
			_flags &= ~(1u << position);
	}

	void Flags::set_bits(int position, int size, char value)
	{
		_flags &= ~(((1u << size) - 1u) << position);
		_flags |= value << position;
	}

	bool Flags::check_bit(const unsigned position) const
	{
		check_argument(position);
		return (_flags >> position) & 1;
	}

	char Flags::get_bit(const unsigned position) const
	{
		check_argument(position);
		return _flags & (1 << position);
	}

	char Flags::get_bits(unsigned position, int size)const
	{
		// Example with position = 2 and size = 3
		// 
		// 01101010b >> 2		= 00011010b
		//
		// Then
		//  1           = 00000001b
		//  1 << 3      = 00001000b
		// (1 << 3) - 1 = 00000111b
		//
		// And we combine the 2 with &
		//
		// 00011010b & 00000111b = 00000010b
		
		return (_flags >> position) & ((1u << size) - 1u);
	}

	void Flags::check_argument(const unsigned position) const
	{
		if (position >= 8)
			throw std::invalid_argument("Position argument can only be between 0 and 8");
	}
}