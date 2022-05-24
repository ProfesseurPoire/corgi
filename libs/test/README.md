# corgi-test

## Introduction

corgi-test is a lightweight C++ header only library which add functionalities to help the user
with testing and TDD. 

## How to Install

Since it's a header only library, all you have to do is to copy the include directory and reference it
inside your project by adding the path through your include_directory.
If you're using CMake to build your project, you could also simply copy the repository inside your project and link 
your project/executable with the corgi-test target

```cmake
    target_link_libraries(your_project corgi-test)
```


## How to use

Use the TEST macro to create a new test. This macro takes 2 parameters. The first correspond to the group the test will belong too,
and the second is the test's name. 

```cpp
#include <corgi/test/test.h>
#include <corgi/math/Vec3f.h>

TEST(Vec3f, Multiplication)
{

}
```

Then, simply write what you want your test to do. In our example, we want to make sure a multiplication between
2 Vector of dimension 3 will output the correct result. In that end, we'll use the CHECK_EQ, CHECK_NE and CHECK macros. Theses macros
takes one or two parameters and make sure the value correspond to what we expected. If not an error is registered
by the testing framework that will be logged inside the terminal, telling you where and why the test failed. 

```cpp
#include <corgi/test/test.h>
#include <corgi/math/Vec3f.h>

TEST(Vec3f, Vec3f)
{
    Vec3f u(1.0f, 2.0f, 3.0f);
    Vec3f u(1.0f, 2.0f, 3.0f);

    CHECK_EQ(u*v, Vec3f(1.0f, 4.0f, 9.0f));
}
```
Once we defined all our test, we just have one more thing to do. Call the corgi::test::run_all() function
from our main. As the name implied, it'll run every test defined by the user.

```cpp

#include<corgi/test/test.>

int main(int argc, char** argv)
{
    return corgi::test::run_all_()
}
```

## Examples

- See the test project inside the repo

