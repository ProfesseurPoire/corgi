#include <corgi/String.h>
#include <corgi/CString.h>

#include <string>

namespace corgi
{

// Lifecycle

    String::String() : _characters(new char[1]), _size(0)
    {
        _characters[0]= '\0';
    }

    String::String(int size) 
        :_characters(new char[size + 1]), _size(size)
    {
        _characters[size] = '\0';
    }

    String::String(const String& str)
        : _characters(new char[str._size+1]), _size(str._size)
    {
        CString::copy(str._characters, _characters);
    }

    String::String(const char* str)
    {
        allocate(CString::size(str));
        CString::copy(str, _characters);
    }

    String::String(const char* str, int size)
        : _characters(new char[size+1]), _size(size)
    {
        CString::copy(str, _characters);
        _characters[_size] = '\0';
    }

    String::~String()
    {
        delete[] _characters;
    }

// Assigment Operators

    String& String::operator+=(const char& c)
    {
        return (*this = *this + c);
    }

    String& String::operator=(const char* str)
    {
        allocate(CString::size(str));
        CString::copy(str, _characters);
        return *this;
    }

    String& String::operator=(const String& str)
    {
        allocate(str.size());
        CString::copy(str._characters, _characters);
        return *this;
    }

    String& String::operator+=(const String& str)
    {
        return (*this = *this + str);
    }

// Comparison Operators

    bool String::operator==(const String& string)const
    {
        for (int i = 0; i < string._size; ++i)
        {
            if (_characters[i] != string._characters[i])
            {
                return false;
            }
        }
        return true;
    }

    bool String::operator==(const char* string)const
    {
        return CString::compare(_characters, string);
    }

    bool String::operator<(const String& str)const
    {
        if (size() == 0 && str.size() == 0)
        {
            return false;
        }

        //The first mismatching means that one string is greater than the other
        // If one string is a prefix of another, the shorter string is less than the other
        for (int i = 0; i < size(); ++i)
        {
            if (i >= str.size())
            {
                // means str is a prefix and smaller
                // Or it could also means they are equals which also return false
                return false;
            }

            if (_characters[i] > str._characters[i])
            {
                return false;
            }

            if (_characters[i] < str._characters[i])
            {
                return true;
            }
        }
        return false;
    }

// Arithmetic Operators

    String String::operator+(const char& c) const
    {
        char* new_characters = new char[_size + 2];

        for(int i=0; i< _size; ++i)
        {
            new_characters[i] = _characters[i];
        }

        new_characters[_size] = c;
        new_characters[_size + 1] = '\0';

        return String(new_characters, _size+1);
    }


    String String::operator+(const String& str)const
    {
        return operator+(str._characters);
    }

    String String::operator+(const char* str)const
    {
        int size = CString::size(str);
        char* newCharacters = new char[_size + size + 1];

        for (int i = 0; i < _size ; ++i)
        {
            newCharacters[i] = _characters[i];
        }

        for (int i = 0; i < size; ++i)
        {
            newCharacters[_size + i ] = str[i];
        }
        newCharacters[_size + size ] = '\0';
        return String(newCharacters);
    }

// Methods

    String String::substr(int start, int length)const
    {
        String result(length);

        // could be using memcpy here I guess
        for (int i = 0; i < length; ++i)
        {
            result._characters[i] = _characters[start + i];
        }

        result._characters[length] = '\0';

        return result;
    }

    String String::substr(int start)
    {
        return substr(start, _size - start);
    }

    void String::allocate(int size)
    {
        delete[] _characters;
        _size		= size;
        _characters	= new char[_size+1]; // +1 for \0
    }

    char* String::data()
    {
        return _characters;
    }

    int String::size()const
    {
        return _size;
    }

    char& String::last()
    {
        return _characters[_size-1];
    }

    char& String::operator[](int index)
    {
        return _characters[index];
    }

    const char& String::operator[](int index)const
    {
        return _characters[index];
    }

    char& String::first()
    {
        return _characters[0];
    }

    bool String::empty()const
    {
        return (size()== 0);
    }

    const char* String::cstr() const
    {
        return _characters;
    }

    void String::resize(int newSize)
    {
        char* newCharacters = new char[newSize + 1];

        for (int i = 0; i < newSize; ++i)
        {
            newCharacters[i] = ' ';
        }

        for (int i = 0; i < _size; ++i)
        {
            newCharacters[i] = _characters[i];
        }

        newCharacters[newSize] = '\0';
        delete[] _characters;
        _characters = newCharacters;
        _size = newSize;
    }

    String String::from_integer(int a)
    {
        return String(std::to_string(a).c_str());
    }
}