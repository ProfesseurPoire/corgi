add_executable(DataAlignment DataAlignment.cpp)

target_link_libraries( DataAlignment PUBLIC CorgiEngine)

set_property(TARGET DataAlignment PROPERTY CXX_STANDARD 20)
