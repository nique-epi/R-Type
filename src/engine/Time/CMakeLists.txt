add_library(rtype_engine_time INTERFACE)

target_include_directories(rtype_engine_time INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(SystemClock)
add_subdirectory(FixedTimestep)
