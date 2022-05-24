#include <corgi/CString.h>

namespace corgi
{
    namespace CString
    {
        bool compare(const char* f, const char* second)
        {
            int i = 0;

            while (f[i] != '\0')
            {
                if (second[i] == '\0')
                {
                    return false;
                }

                if (f[i] != second[i])
                {
                    return false;
                }
                i++;
            }
            return second[i] == '\0';
        }

        bool text_match(const char* str, const char* text, bool case_sensitive)
        {
            int i = 0;

            while (str[i] != '\0')
            {
                if (text[i] == '\0')
                {
                    // Means we parsed the whole text
                    // and everything was exactly the same
                    return true;
                }
                else
                {
                    if (case_sensitive==false)
                    {
                        // Is Upper Case
                        if(text[i]>=65 && text[i] <=90)
                        {
                             if(text[i] != str[i] && (text[i]+32) !=str[i])
                             {
                                 return false;
                             }
                        }
                        // Is Lower case
                        else if(text[i]>=97 && text[i]<=122)
                        {
                            if (text[i] != str[i] && (text[i]-32) != str[i])
                            {
                                return false;
                            }
                        }
                    }
                    else if (text[i] != str[i])
                    {
                        return false;
                    }
                }
                i++;
            }
            // Means we parsed str entirely
            return true;
        }

        int size(const char* str)
        {
            int i = 0;
            while (str[i] != '\0')
            {
                i++;
            }
            return i;
        }

        void copy(const char* input, char* output)
        {
            int i = 0;
            // Could probably be done with a memcpy too
            // with more information about the strings
            while (input[i] != '\0')
            {
                output[i] = input[i];
                i++;
            }
            output[i] = '\0';
        }

    }
}
