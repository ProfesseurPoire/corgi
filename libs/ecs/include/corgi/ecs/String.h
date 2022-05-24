#pragma once

namespace corgi
{
	class String
	{
	public:

		String();
		String(char* str);
		~String();
		

		String(const String& str);
		String(String&& str);

		String& operator=(const String& str);
		String& operator=(String&& str);

		String& operator=(const char* str);

		const char* c_str();

	private:

		char* _string;
	};
}