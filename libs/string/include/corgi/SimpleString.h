#pragma once
#include "SimpleString.h"

#include <iostream>
#include <vector>

namespace corgi
{
    /*!
	 *	@brief	SimpleString doesn't keep tracks of the
	 *			string's length, and only stores a char*
	 */
    class SimpleString
    {
    public:
        static constexpr int npos = -1;

        static SimpleString fromInteger(int number);

        // Constructors

        SimpleString(const char* str = "");

        ~SimpleString();

        SimpleString(const SimpleString& str);
        SimpleString(SimpleString&& str) noexcept;

        SimpleString& operator=(const SimpleString& str);
        SimpleString& operator=(SimpleString&& str) noexcept;

        SimpleString& operator=(const char* str);
        SimpleString& operator=(char c);

        // Functions

        void               clear();
        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] int  size() const;

        char operator[](int index) const;

        /**
         * @brief Removes the character at index
         * 
         * @param index 
         */
        void remove(int index);

        SimpleString& operator+=(char c);
        SimpleString& operator+=(const char* c);
        SimpleString& operator+=(const SimpleString& str);

        SimpleString operator+(char c) const;
        SimpleString operator+(const char* c) const;
        SimpleString operator+(const SimpleString& str) const;

        // TODO : Real implementation of this
        int find(const char* str);

        char front() const;

        /**
         * @brief Adds the character at the front
         * 
         * @param c 
         */
        void addFront(char c);

        /**
         * @brief   Returns a substring that start at @a startPosition of @a length characters
         * 
         * @param startPosition starting position of the returned string in the original string
         * @param length how many characters are included from the original string
         * 
         * @return SimpleString 
         */
        SimpleString substr(int startPosition, int length) const;

        /**
         * @brief   Returns a substring that starts at @a startPosition to the end of the original string
         *          
         *          The returned string starts at @a startPosition and ends at the end of the original string
         * 
         * @param   startPosition starting position of the returned string in the original string
         * 
         * @return SimpleString 
         */
        SimpleString substr(int startPosition) const;

        std::vector<SimpleString> split(char delimiter);

        SimpleString replace(SimpleString characters, SimpleString replacedCharacters);

        // Conversions

        /*!
         * @brief   Tries to convert the string to an int
         */
        int toInt() const;

        /*!
         * @brief   Tries to convert the string to a float
         */
        float toFloat() const;

        // Comparison operators

        bool               operator<(const SimpleString& str) const;
        [[nodiscard]] bool operator==(const char* str) const;
        [[nodiscard]] bool operator==(const SimpleString& str) const;

        [[nodiscard]] bool operator!=(const char* str) const;
        [[nodiscard]] bool operator!=(const SimpleString& str) const;

        [[nodiscard]] const char* c_str() const noexcept;

        char* begin();
        char* end();

        char* begin() const;
        char* end() const;

        friend std::ostream& operator<<(std::ostream& in, const SimpleString& c1)
        {
            in << c1.c_str();    // This is calling this function infinitely!!
            return in;
        }

    private:
        void copy_string(const char* str);

        char* _string;
    };

    SimpleString operator+(const char*, const SimpleString& str);
    SimpleString to_string(int number);
    SimpleString to_string(float number);
    SimpleString to_string(unsigned number);

    float to_float(const SimpleString& str);
    int   to_int(const SimpleString& str);
}    // namespace corgi
