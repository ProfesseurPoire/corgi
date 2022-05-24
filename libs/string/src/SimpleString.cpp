#include <corgi/SimpleString.h>

// I keep using this because strlen is quite complicated
// actually and plays a lot with bit manipulations

#include <cstring>

// Using string here mostly for the to_string function
#include <string>

using namespace corgi;

/// <summary>
/// Count how many character are inside the string up until the null terminating character
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
static constexpr char null_character = '\0';

SimpleString SimpleString::fromInteger(int number)
{
    return SimpleString(std::to_string(number).c_str());
}

SimpleString::~SimpleString()
{
    delete[] _string;
}

SimpleString::SimpleString(const SimpleString& str)
{
    copy_string(str.c_str());
}

SimpleString SimpleString::replace(SimpleString characters,
                                   SimpleString replacedCharacters)
{
    std::string str(_string);
    size_t      index;

    while((index = str.find(characters.c_str())) != std::string::npos)
    {
        std::string before = str.substr(0, index);
        std::string after  = str.substr(index + characters.size(), std::string::npos);

        str = before + std::string(replacedCharacters.c_str()) + after;
    }
    return str.c_str();
}

SimpleString::SimpleString(SimpleString&& str) noexcept
{
    _string     = str._string;
    str._string = nullptr;
}

SimpleString& SimpleString::operator=(const SimpleString& str)
{
    SimpleString cpy(str);
    *this = std::move(cpy);
    return *this;
}

SimpleString& SimpleString::operator=(SimpleString&& str) noexcept
{
    delete[] _string;
    _string     = str._string;
    str._string = nullptr;
    return *this;
}

void SimpleString::remove(int index)
{
    auto string = _string;
    auto length = size();

    char* newString = new char[length];

    int j = 0;

    for(int i = 0; i < (length + 1); i++)
    {
        if(i != index)
        {
            newString[j] = string[i];
            j++;
        }
    }
    delete _string;
    _string = newString;
}

SimpleString::SimpleString(const char* str)
{
    copy_string(str);
}

void SimpleString::addFront(const char c)
{
    int new_size = strlen(_string) + 1;

    char* new_str = new char[new_size + 1];
    memcpy(new_str + 1, _string, new_size);

    new_str[0]        = c;
    new_str[new_size] = '\0';

    delete[] _string;
    _string = new_str;
}

SimpleString& SimpleString::operator=(const char* str)
{
    delete[] _string;
    copy_string(str);
    return *this;
}

SimpleString& SimpleString::operator=(char c)
{
    delete[] _string;
    _string    = new char[2];
    _string[0] = c;
    _string[1] = '\0';
    return *this;
}

void SimpleString::clear()
{
    *this = "";
}

bool SimpleString::empty() const noexcept
{
    return strlen(_string) == 0;
}

bool SimpleString::operator<(const SimpleString& str) const
{
    return strcmp(_string, str._string) < 0;
}

int SimpleString::size() const
{
    return strlen(_string);
}

char SimpleString::operator[](int index) const
{
    return _string[index];
}

SimpleString& SimpleString::operator+=(char c)
{
    int new_size = strlen(_string) + 1;

    char* new_str = new char[new_size + 1];
    memcpy(new_str, _string, new_size);

    new_str[new_size - 1] = c;
    new_str[new_size]     = '\0';

    delete[] _string;
    _string = new_str;
    return *this;
}

SimpleString SimpleString::operator+(char c) const
{
    SimpleString str(*this);
    return str += c;
}

SimpleString& SimpleString::operator+=(const char* c)
{
    int sizea = strlen(_string);
    int sizeb = strlen(c);

    char* new_str = new char[sizea + sizeb + 1];
    new_str[0]    = '\0';
    strcat(new_str, _string);
    strcat(new_str, c);

    delete[] _string;

    _string = new_str;
    return *this;
}

SimpleString& SimpleString::operator+=(const SimpleString& str)
{
    return operator+=(str.c_str());
}

SimpleString SimpleString::operator+(const char* c) const
{
    SimpleString str(*this);
    str += c;
    return str;
}

SimpleString SimpleString::operator+(const SimpleString& str) const
{
    SimpleString s(*this);
    s += str;
    return s;
}

int SimpleString::find(const char* str)
{
    std::string s(_string);
    return s.find(str);
}

char SimpleString::front() const
{
    return _string[0];
}

int SimpleString::toInt() const
{
    return std::stoi(std::string(_string));
}

float SimpleString::toFloat() const
{
    return std::stof(std::string(_string));
}

SimpleString SimpleString::substr(int start, int length) const
{
    if(length == npos)
    {
        length = strlen(_string) - start;
    }

    char* new_string = new char[length + 1];
    memcpy(new_string, &_string[start], length);
    new_string[length] = '\0';
    return SimpleString(new_string);
}

SimpleString SimpleString::substr(int startPosition) const
{
    return substr(startPosition, size() - startPosition);
}

std::vector<SimpleString> SimpleString::split(char delimiter)
{
    int i = 0;

    std::vector<SimpleString> result;

    SimpleString str;

    while(_string[i] != '\0')
    {
        if(_string[i] != delimiter)
        {
            str += _string[i];
        }
        else
        {
            result.push_back(str);
            str.clear();
        }
        i++;
    }
    return result;
}

bool SimpleString::operator==(const char* str) const
{
    return strcmp(_string, str) == 0;
}

bool SimpleString::operator==(const SimpleString& str) const
{
    return strcmp(_string, str._string) == 0;
}

bool SimpleString::operator!=(const char* str) const
{
    return !operator==(str);
}

bool SimpleString::operator!=(const SimpleString& str) const
{
    return !operator==(str);
}

const char* SimpleString::c_str() const noexcept
{
    return _string;
}

char* SimpleString::begin()
{
    return _string;
}

char* SimpleString::end()
{
    return &_string[strlen(_string)];
}

char* SimpleString::begin() const
{
    return _string;
}

char* SimpleString::end() const
{
    return &_string[strlen(_string)];
}

void SimpleString::copy_string(const char* str)
{
    const auto size = strlen(str);
    _string         = new char[size + 1];
    memcpy(_string, str, size);
    _string[size] = '\0';
}

SimpleString corgi::operator+(const char* string, const SimpleString& str)
{
    SimpleString s(string);
    return s + str;
}

SimpleString corgi::to_string(const int number)
{
    return SimpleString(std::to_string(number).c_str());
}

SimpleString corgi::to_string(const float number)
{
    return SimpleString(std::to_string(number).c_str());
}

SimpleString corgi::to_string(const unsigned number)
{
    return SimpleString(std::to_string(number).c_str());
}

float corgi::to_float(const SimpleString& str)
{
    std::string s(str.c_str());
    return std::stof(s);
}

int corgi::to_int(const SimpleString& str)
{
    std::string s(str.c_str());
    return std::stoi(s);
}
