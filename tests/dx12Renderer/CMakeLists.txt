cmake_minimum_required(VERSION 3.8.0)

project(dx12Renderer)

# Tells cmake to use relative path for the target_sources command
cmake_policy(SET CMP0076 NEW)

add_executable(${PROJECT_NAME} "")
add_subdirectory(src)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_compile_definitions(${PROJECT_NAME} PUBLIC CORGI_VERBOSITY=3)

if(MSVC)
    # Removing annoying CRT warnings on msvc
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4514")
endif()

if(UNIX)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC d3d12.lib
    dxgi.lib
    dxguid.lib)
