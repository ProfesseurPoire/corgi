cmake_minimum_required (VERSION 3.13.0)

add_executable(test-corgi-test "") 

# I'm not sure I understand all the advantages on using target_sources but at least
# it's one way to avoid adding variables, and to define our target early
target_sources(test-corgi-test 
   PUBLIC 
       main.cpp 
       test_fixture.cpp
       TestA.cpp 
       TestB.cpp
       TestTime.cpp)

if(NOT CorgiTest)
    #find_package(CorgiTest)
endif()

target_link_libraries(test-corgi-test CorgiTest)

set_property(TARGET test-corgi-test  PROPERTY CXX_STANDARD 20)

add_test( NAME test-corgi-test COMMAND test-corgi-test)
