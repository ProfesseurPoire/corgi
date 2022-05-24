cmake_minimum_required(VERSION 3.13.0)

project(CorgiUtils DESCRIPTION "Library handling various common thing accross the engine" LANGUAGES CXX VERSION 1.0.0)

add_library(${PROJECT_NAME} "")

add_subdirectory(src)
add_subdirectory(include/corgi/utils)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC include
	${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiEngine)
