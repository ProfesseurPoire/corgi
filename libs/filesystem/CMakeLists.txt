cmake_minimum_required(VERSION 3.9.0)

project(corgi-filesystem VERSION 1.0.0)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

add_library(${PROJECT_NAME} STATIC
	src/FileSystem.cpp
    src/Document.cpp)

set_property(TARGET corgi-filesystem PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} CorgiContainers corgiString)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:../>)

