cmake_minimum_required(VERSION 3.9.0)

project(corgiString)

add_library(${PROJECT_NAME} STATIC
    src/String.cpp
    src/CString.cpp
    src/SimpleString.cpp
 "include/corgi/SimpleString.h" "src/SimpleString.cpp" "include/corgi/ShortString.h" "src/ShortString.cpp")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)


add_subdirectory(tests/unitTest)
