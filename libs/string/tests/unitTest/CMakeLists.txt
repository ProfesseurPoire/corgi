cmake_minimum_required(VERSION 3.9.0)

project(UnitTestString)

add_executable(${PROJECT_NAME}
    main.cpp
    UTString.cpp
    UTCString.cpp
 UTShortString.cpp)

target_link_libraries(${PROJECT_NAME}
    CorgiTest
    corgiString
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})