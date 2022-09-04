cmake_minimum_required(VERSION 3.15.0)

project(CorgiEngine LANGUAGES CXX VERSION 1.0.0)

include(conan/conanbuildinfo.cmake)
conan_basic_setup()

option(BUILD_TESTS "Build the tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_library(${PROJECT_NAME} "")

add_subdirectory(ext)
add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(include/corgi)
add_subdirectory(libs)
add_subdirectory(libs/components/src)
add_subdirectory(libs/components/include/corgi/components)

target_precompile_headers(${PROJECT_NAME}
    PUBLIC
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>")

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

configure_file(src/config.h.in config.h)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/components/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIRECTORY} # Will need to be purged later on
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

# We add that because SDL2_mixer call headers directly from SDL2
if(UNIX)
    target_include_directories(${PROJECT_NAME} PUBLIC /usr/include/SDL2)
endif()

if(${MSVC_CLANG})
    message(WARNING "Entering CLANG Compilation Settings")

    # -Wno-c++98-compat-pedantic removes a warning triggered when using C++ features that are not
    # compatible with c++ 98 but that's ok cause we're not targeting it

    # -Wshadow-field-in-constructor is just a warning that procs in constructors
    # where the constructor parameter has the same name of a member variable.
    # I find it mostly annoying and if I really need to differentiate the
    # 2 variables I'd use the this keyword anyways, instead of chaning the
    # whole naming convention and adding a p prefix to parameters

    # -Wfloat-equal triggers for every == float comparison, which I think
    # is overkill because most of the time I want to check if 2 float are
    # actually the same, and not the same after some computation
    # If comparing float values coming from a computation, I'd need
    # an epsilon
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-c++98-compat-pedantic -Wshadow-field-in-constructor -Wno-float-equal)
elseif(MSVC)
    message(STATUS "Entering MSVC Compilation Settings")

    # target_compile_options(${PROJECT_NAME} PRIVATE /W3)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4514 /wd5045")
else()
    message(STATUS "Entering LINUX Compilation Settings")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# Link Libraries
target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS})

target_link_libraries(${PROJECT_NAME} PUBLIC
    corgi-ecs
    corgi-math
    corgi-filesystem
    CorgiContainers
    CorgiUi
    CorgiLogger
    RapidJson
    CorgiUtils)

if(MSVC)
    target_link_libraries(${PROJECT_NAME} PUBLIC opengl32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC GL stdc++fs)
endif()