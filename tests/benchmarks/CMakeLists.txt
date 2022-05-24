cmake_minimum_required(VERSION 3.10.0)
project(CorgiBenchmarks)

add_executable(${PROJECT_NAME} src/CorgiBenchmarks.cpp )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

set_target_properties(${PROJECT_NAME} PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiEngine)

target_include_directories(${PROJECT_NAME} PUBLIC include/)



# Only needed for a release build 
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD    # Adds a post build event to the given target
#    COMMENT "Copying "
#    COMMAND ${CMAKE_COMMAND} -E copy_directory          # Executes the copy directory command
#        "${CMAKE_CURRENT_SOURCE_DIR}/resources"   
#        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD    # Adds a post build event to the given target
    COMMAND ${CMAKE_COMMAND} -E copy_directory          # Executes the copy directory command
        "${CMAKE_SOURCE_DIR}/libs/corgi/resources"   
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
    COMMENT "Copy Library resources")

add_custom_target(
    RunCorgiBenchmark ALL
    COMMAND ResourcesPackager -I ${CMAKE_SOURCE_DIR}/libs/corgi/resources ${CMAKE_CURRENT_SOURCE_DIR}/resources/ -O ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMENT "Running Resources Packager application")

add_dependencies(${PROJECT_NAME} RunCorgiBenchmark)