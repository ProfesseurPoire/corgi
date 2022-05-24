cmake_minimum_required(VERSION 3.8.0)

project(ResourcesCacheTests)

configure_file(config.h.in config.h)

add_executable(${PROJECT_NAME} src/ResourcesCacheTests.cpp)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W3 )
endif()

if(UNIX)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic )
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} PUBLIC CorgiEngine CorgiTest)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR})

add_test( NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})



add_custom_target(
    run ALL
    COMMAND ResourcesPackager -I ${CMAKE_SOURCE_DIR}/libs/corgi/resources ${CMAKE_CURRENT_SOURCE_DIR}/resources/ -O ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMENT "Running Resources Packager application")

add_dependencies(${PROJECT_NAME} run)