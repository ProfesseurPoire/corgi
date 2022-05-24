project(TestMultithreading)

add_executable(${PROJECT_NAME} TestMultithreading.cpp)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    CorgiEngine
    CorgiTest)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)