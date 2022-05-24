project(TestECS)

enable_testing()

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../../ext/boost/include)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corgi-ecs
    CorgiTest)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
