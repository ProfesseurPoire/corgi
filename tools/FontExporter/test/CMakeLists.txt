project(FontExporterTest)

add_executable(${PROJECT_NAME} src/FontExporterTest.cpp)

configure_file(config.h.in config.h)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries( ${PROJECT_NAME} PUBLIC 
    CorgiTest
    FontExporter)