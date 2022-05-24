#pragma once

namespace corgi
{
	/*!
	 * @brief	Use this is you want to manipulate bits directly
	 *			Useful when you want each bit to represents something
	 *			like an enumeration
	 */
	class Flags
	{
	public:

		// Lifecycle
		
		Flags() = default;
		Flags(char c);

		// Functions

		[[nodiscard]] bool operator==(const Flags& other)const noexcept;
		[[nodiscard]] bool operator!=(const Flags& other)const noexcept;

		[[nodiscard]] bool operator <(const Flags& other)const noexcept;

		/*!
		 * @brief	Sets the bit at @a pos 
		 */
		void set_bit(char position, bool value);

		/*!
		 * @brief	Sets the bits at @a position with @a size bits from @a value
		 *
		 *			Example with 00000011b in value
		 *
		 *			set_bits(2,2,value)
		 *
		 *			with 10101010b in flag
		 *							 __   <- affected bits
		 *			-> will give 10101110b
		 */
		void set_bits(int position, int size, char value);

		/*!
		 * @brief	Returns the state of the @a pos bit
		 */
		[[nodiscard]] bool check_bit(unsigned position) const;

		/*!
		 * @brief	Returns a char with only the @a pos state bit
		 */
		[[nodiscard]] char get_bit(unsigned position) const;

		/*!
		 * @brief	Returns a char with @a size bits startint at @a position
		 */
		[[nodiscard]] char get_bits(unsigned position, int size)const;

	private:

		void check_argument(unsigned position) const;

		char _flags = 0b00000000;
	};
}
