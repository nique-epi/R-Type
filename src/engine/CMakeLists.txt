add_subdirectory(Exceptions)
add_subdirectory(Time)

add_library(rtype_engine STATIC EntityRegistry.cpp)

target_include_directories(rtype_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine
  PUBLIC
    rtype_engine_exceptions
    rtype_engine_time
    rtype_engine_system_clock
    rtype_engine_fixed_timestep
)

rtype_enable_warnings(rtype_engine)
