# add_executable(PerformanceTests main.cpp)

# add_executable(WindowDrawListTest WindowDrawListTest.cpp)
# add_executable(UnitTests UnitTests.cpp)

# target_link_libraries( WindowDrawListTest
# PUBLIC
# corgiEngine
# )

# target_link_libraries(PerformanceTests PUBLIC CorgiTest)

# set_property(TARGET PerformanceTests PROPERTY CXX_STANDARD 17)
# set_property(TARGET WindowDrawListTest PROPERTY CXX_STANDARD 17)
# set_property(TARGET UnitTests PROPERTY CXX_STANDARD 17)

add_subdirectory(data_alignment)
add_subdirectory(multithreading)
add_subdirectory(resources_cache)
add_subdirectory(benchmarks)
add_subdirectory(dx12Renderer)
add_subdirectory(vulkanRenderer)