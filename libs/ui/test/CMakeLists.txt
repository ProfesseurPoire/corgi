project(TestUI)

add_executable(${PROJECT_NAME} src/TestUi.cpp)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

configure_file(config.h.in config.h)

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiEngine CorgiUi)

target_include_directories(${PROJECT_NAME} PUBLIC include/ ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                # Adds a post build event to the given target
    COMMAND ${CMAKE_COMMAND} -E copy_directory                      # Executes the copy directory command
        "${CMAKE_SOURCE_DIR}/resources"   
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD                # Adds a post build event to the given target
    COMMAND ${CMAKE_COMMAND} -E copy_directory                      # Executes the copy directory command
        "${CMAKE_SOURCE_DIR}/libs/corgi/resources"   
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

add_custom_target(
    runnnn ALL
    COMMAND ResourcesPackager -I ${CMAKE_SOURCE_DIR}/libs/corgi/resources ${CMAKE_CURRENT_SOURCE_DIR}/resources/ -O ${CMAKE_CURRENT_SOURCE_DIR}/src/
    COMMENT "Running Resources Packager application")

add_dependencies(${PROJECT_NAME} runnnn)