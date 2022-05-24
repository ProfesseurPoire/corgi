cmake_minimum_required (VERSION 3.13.0)

project(CorgiTest VERSION 1.0.0)

set(CMAKE_DEBUG_POSTFIX d)

# We'll install the library in lib64 if we're in x64, in lib otherwise
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(arch_suffix "")
else()
    set(arch_suffix "64")
endif()

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME}
INTERFACE 
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
$<INSTALL_INTERFACE:lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/include/>)

#####################################################
# Installing the library and exporting the targets
#####################################################

include(CMakePackageConfigHelpers)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-target
    LIBRARY     DESTINATION lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/
    ARCHIVE     DESTINATION lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/
    RUNTIME     DESTINATION lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/
    INCLUDES    DESTINATION lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/include)

install(
    EXPORT  ${PROJECT_NAME}-target
    DESTINATION lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/cmake)

configure_file(cmake/config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)

# Installs the config and config version file 
install(FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"  
    DESTINATION lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/cmake)

# Installing header files
install(DIRECTORY include/
    DESTINATION lib${arch_suffix}/${PROJECT_NAME}-${PROJECT_VERSION}/include
    FILES_MATCHING PATTERN *.h)

#########################################
# Tests
#########################################

option(BUILD_TESTS "Build the corgi test tests" ON)

if(BUILD_TESTS)
enable_testing()
add_subdirectory(test)
endif()