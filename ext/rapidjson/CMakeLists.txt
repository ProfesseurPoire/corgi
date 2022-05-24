add_library(RapidJson INTERFACE)

# We add the SYSTEM keyword so we don't have warnings from this library when we compile our thing
target_include_directories(RapidJson SYSTEM INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rapidjson)