cmake_minimum_required(VERSION 3.9.0)

project(corgi-math VERSION 1.0.0)

# Need that for some locality shenanigans
cmake_policy(SET CMP0076 NEW) 

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)


add_library(${PROJECT_NAME} STATIC
	Matrix.cpp
	MathUtils.cpp
	MathUtils.h
	rectangle.h
	rectanglef.h
	Ray.cpp
	Collisions.cpp)


	
#Disable warning 5045 : Spectre Warning : I have no idea how to actually fix
# these warning, nor why they occur in the first place
if(${MSVC_CLANG})
    message(WARNING "Entering CLANG Math Compilation Settings")
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
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4514 /wd5045")
else()
    message(STATUS "Entering LINUX Compilation Settings")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

add_subdirectory(src)

set_property(TARGET corgi-math PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../>
    $<INSTALL_INTERFACE:../../>)

add_subdirectory(UnitTests)

