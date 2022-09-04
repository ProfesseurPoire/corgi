project(FontExporter)

add_library(${PROJECT_NAME} STATIC "")

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CONAN_LIBS_FREETYPE})
target_link_libraries(${PROJECT_NAME} PUBLIC CorgiEngine corgi-filesystem RapidJson)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_subdirectory(test)