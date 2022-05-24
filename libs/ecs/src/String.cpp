//#include <corgi/ecs/String.h>
//
//
//using namespace corgi;
//
///// <summary>
///// Count how many character are inside the string up until the null terminating character
///// </summary>
///// <param name="str"></param>
///// <returns></returns>
//static int strlen(const char* str)
//{
//	int i = 0;
//	while (str[i] != '\0')
//		i++;
//	return i;
//}
//
//static int strcpy(const char* src, char* dest)
//{
//	// Could also use memcpy but it probably evens out, but I'll try to make some test around that 
//	int i = 0;
//	while (src[i] != '\0')
//	{
//		dest[i] = src[i];
//		i++;
//	}
//}
//
//String::String(){}
//
//String::String(char* str)
//	: _string(new char[strlen(str)])
//{
//
//}