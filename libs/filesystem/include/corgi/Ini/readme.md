# Table of Contents
1. [Introduction](#Introduction)
2. [INI file format](#The\ INI\ file\ format)
3. [How to install](#How\ to\ install)
4. [How to use](#How\ to\ use)
    1. [Reading](##Reading)
    2. [Writing](##Writing)
5. [How it works](#third-example)
6. [License](#License)

# Introduction

IniDocument is a C++ 17 library that provides facilities to interact with files
that written in INI format

# The INI file format 

 An INI file is a plain text file format composed of sections, keys and
 values mostly used to create configuration files.

 For more information about the INI file format: https://en.wikipedia.org/wiki/INI_file
 
## Keys

 A key associate a name and a value together and is stored on 1 line. 
 The name and the value are delimited by an equal sign
 
 ```ini
name = value 
 ```
 
## Sections

 A key can be grouped into a section. A section is stored on 1 line 
 in square brackets. Every key declared after a section is 
 associated with that section.
 
 ```ini
[section]
key1=value
 ```
 
## Comments
 
 Ini files also support comments. To create a comment, simply start 
 a line with the ';' character 

## INI file example

  ```ini
name    = value
stuff   = thing
; Here's a comment
[first section]
another thing = value
  ```

# How to use

## Reading

To retrieve data from an existing ini file, use the read 
function with the path to the ini file as an argument. If everything went right,
the function should return true.

Then, use the value function to fetch the value of an existing key. Then you can use the to_functions to convert
the data to a common type.

```cpp
#include <ini/document.h>

int main(int argc, char** argv)
{
    int width{0};
    std::string name{""};

    Ini::Document document;

    if(document.read("file"))
    {
        width   = document.value("width")->to_int();
        name    = document.value("name","character")->to_string();
    } 
}
```

## Writing

To save data, you should first create a new instance of the class through
the setters functions value() functions.
Once you registered all your information, use the write function to save 
the data in a INI file.

```cpp
IniDocument document;
document.value(10, "key_int","camera_section");
document.value("value", "key_string");
document.write("file");
```

# How to install


  
# How it works

An instance of Ini::Document stores the data inside a map that 
associate a  section(string) with another map that associate a key
(string), with a value. A value is an object that offers helper 
functions to transform a string in common data type (int, float, bool..)

Keys without a section are grouped under the @ref default_section

# License 

This project is licensed under the MIT License - see the LICENSE.md file for details