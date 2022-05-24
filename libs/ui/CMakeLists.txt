cmake_minimum_required(VERSION 3.13.0)

project(CorgiUi DESCRIPTION "Library for UI stuff" LANGUAGES CXX VERSION 1.0.0)

add_library(${PROJECT_NAME} "")

add_subdirectory(src)
add_subdirectory(include/corgi/ui)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC include
	${CMAKE_CURRENT_BINARY_DIR}
)


target_precompile_headers(${PROJECT_NAME}
 PUBLIC 
 "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/src/ui_pch.h>")

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiEngine RapidJson)

add_subdirectory(test)