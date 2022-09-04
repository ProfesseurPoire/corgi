add_executable(ResourcesPackager ResourcesPackager.cpp TiledImporter.h TiledImporter.cpp)

set_property(TARGET ResourcesPackager PROPERTY CXX_STANDARD 20)

target_link_libraries(ResourcesPackager PUBLIC CorgiEngine FontExporter)
