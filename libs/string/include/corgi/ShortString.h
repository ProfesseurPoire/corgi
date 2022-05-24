#pragma once

namespace corgi
{
	class ShortString
	{
	public:

		ShortString(const char* str="");
		[[nodiscard]] const char* c_str()const;


		ShortString& operator=(const char* str);

		[[nodiscard]] bool operator==(const ShortString& other)const noexcept;
		[[nodiscard]] bool operator!=(const ShortString& other)const noexcept;
		[[nodiscard]] bool operator<(const ShortString& other)const noexcept;


	private:

		char _str[64];
	};

}
