cmake_minimum_required(VERSION 3.13.0) # need 3.13 for target_sources

project(TestContainers CXX)

add_executable(${PROJECT_NAME} "")
add_subdirectory(unit)
    
# Needed so it looks for CorgiTest in lib64 folder when in x64 architecture
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    CorgiContainers
    CorgiTest)

target_include_directories(TestContainers PRIVATE ./)

set_property(TARGET TestContainers PROPERTY CXX_STANDARD 20)

add_test(NAME TestContainers COMMAND TestContainers)
