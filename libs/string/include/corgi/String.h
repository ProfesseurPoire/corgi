#pragma once

#include <string>

// TODO : For now just try to remove this everywhere it is actually used
namespace corgi
{
    class String 
    {

    public:

    // Static

        /*void switch_character(std::string& str, char old, char newChar)
        {
            for (auto& c : str)
            {
                if (c == old)
                {
                    c = newChar;
                }
            }
        }*/

        static String from_integer(int a);

    // Lifecycle

        /*
         * @brief   Creates an empty string
         */
        String();

        /*
         * @brief   Creates an empty string and allocate enough memory
         *          for @ref size characters
         *
         *          Use this constructor when you know the size of 
         *          string and want to avoid multiple memory allocations
         *
         * @param   size    How many characters (and memory) we expect
         *                  the string to store
         */
        explicit String(int size);

        /*
         * @brief   Copy the given C-style string
         * 
         *          The string will be copied character by character
         *          until a null terminated character is found
         * 
         * @param   str     C-style string that will be copied
         */
        String(const char* str);


        /*
         * @brief   Copy the given C-style string
         * 
         *          The string will be copied character by character
         *          until a null terminated character is found. The size
         *          parameter allow us to not directly allocate the 
         *          string without having to parse it to know its size
         *          
         * @param   str     C Style string that will be copied
         * @param   size    Size of the C Style string that will be copied
         */
        String(const char* str, int size);

        /*
         * @brief   Copy the given String into the current String object
         * 
         * @param   str     The string to be copied 
         */
        String(const String& str);


       // String(String&& str) = delete;

        ~String();

    // Assignment Operators

        String& operator=(const String& str);
       // String& operator=(String&& str) = delete;
        String& operator=(const char* string);

        String& operator+=(const String& str);

        /*
         * @brief   Concatenate the current string with the given character,
         *          and returns a reference to the current string
         *          
         * @param   c   Character to be appended to the current string
         * 
         * @returns Returns a reference to the current string object
         */
        String& operator+=(const char& c);
        
        
    // Comparison Operators

        bool operator==(const String& str) const;
        bool operator==(const char* str) const;

        // lexicographical comparison
        // see https://en.cppreference.com/w/cpp/algorithm/lexicographical_compare
        bool operator<(const String& str) const;

    // Arithmetic Operators

        String operator+(const char* str) const;
        String operator+(const String& str) const;

        /*
         * @brief   Returns a new string character that concatenate the
         *          current string with the given character.
         *          
         * @param   c    Character to be appended to the current string
         * 
         * @returns Returns a new String character object
         */
        String operator+(const char& c)const;

    // Methods

        /*
         * @brief   Generates a substring from the current string
         *
         * @param   start   Index where the new string starts
         * @param   length  Length of the substring
         *
         * return   Returns a new String object 
         */
        String substr(int start, int length)const;

        /*
         * @brief   Generates a substring from the current string
         *
         *          This function will return a string that starts
         *          at the given index to the end of the current
         *          string
         *
         * @param   start   Index where the new string starts
         *
         * @return  Returns a new String object
         */
        String substr(int start);

        /*
         * @brief   Returns a pointer to the underlying array
         *
         *          This function is useful when you need to work
         *          with C style string functions
         *
         * @return  Returns a pointer to the first char in the string
         */
        char* data();

        /*
         * @brief   Returns a reference to the first character
         */
        char& first();

        /*
         * @brief   Returns a reference to the last character
         */
        char& last();
        
        /*
         * @brief   Returns a reference to the character at the given position
         *          inside the string
         *
         *          Warning : No boundary checks are made
         *
         * @param   pos     Position of the character inside the string
         *
         * @return  Returns a reference to the character
         */
        char& operator[](int pos);

        /*
         * @brief   Returns a const reference to the character at the given 
         *          index
         *          
         * @param   index   Index of the character inside the array/string
         */
        const char& operator[](int position)const;

        /*
         * @brief Returns the string as a const char*
         */
        const char* cstr() const;

        /*
         *	@brief Returns how many character the string holds
         *
         *	Warning : doesn't count the null terminator character
         */
        int size() const;

        /*
         * @brief   Returns true if the string is empty
         * 
         *          An empty string is a string without any character
         *          like ""
         *          
         * @return  Returns true if empty, false otherwise
         */
        bool empty() const;

        /*
         * @brief   Allocate memory to the string if needed
         * 
         *          Typically you would use this function if you need to add
         *          one character at a time to avoid unnecessary memory
         *          reallocation
         * 
         */
        void resize(int new_size);

    private:

        char*   _characters = nullptr;
        int     _size       = 0;

        void allocate(int size);
    };

    inline String operator+(const char* lhs, const String& rhs)
    {
        String str = lhs;
        return (str + rhs);
    }
}
