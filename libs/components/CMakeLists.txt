cmake_minimum_required(VERSION 3.9.0)

project(CorgiComponents VERSION 1.0.0)

cmake_policy(SET CMP0076 NEW) 

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

add_library(${PROJECT_NAME} "")
add_subdirectory(src)
add_subdirectory(include/corgi/components)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} CorgiContainers corgi-ecs)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
