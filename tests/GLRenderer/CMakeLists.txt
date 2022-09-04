cmake_minimum_required(VERSION 3.16.0)

project(GLRenderer)

# Tells cmake to use relative path for the target_sources command
cmake_policy(SET CMP0076 NEW)

add_executable(${PROJECT_NAME} GLRendererMain.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_compile_definitions(${PROJECT_NAME} PUBLIC CORGI_VERBOSITY=3)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # Not sure why this doesnt works on visual studio. I did bump cmake version
    # so it should work, and it works with visual studio code
    # target_compile_options(${PROJECT_NAME} PRIVATE /W4)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4514")
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD # Adds a post build event to the given target
    COMMAND ${CMAKE_COMMAND} -E copy_directory # Executes the copy directory command
    "${PROJECT_SOURCE_DIR}/shaders"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD # Adds a post build event to the given target
    COMMAND ${CMAKE_COMMAND} -E copy_directory # Executes the copy directory command
    "${PROJECT_SOURCE_DIR}/resources"
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

if(UNIX)
    # target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS} CorgiEngine corgi-math ${VULKAN_SDK_PATH}/Lib/vulkan-1.lib)

add_custom_target(
    RunResourcePackagerGL ALL
    COMMAND ResourcesPackager -I ${CMAKE_CURRENT_SOURCE_DIR}/resources/ -O ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMENT "Running Resource Packager")

add_dependencies(${PROJECT_NAME} RunResourcePackagerGL)
