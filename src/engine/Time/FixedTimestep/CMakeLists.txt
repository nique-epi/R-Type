add_library(rtype_engine_fixed_timestep STATIC FixedTimestep.cpp)

target_include_directories(rtype_engine_fixed_timestep PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_fixed_timestep
  PUBLIC
    rtype_engine_time
    rtype_engine_exceptions
)

rtype_enable_warnings(rtype_engine_fixed_timestep)
