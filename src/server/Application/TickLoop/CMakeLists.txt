add_library(rtype_server_tick_loop STATIC TickLoop.cpp)

target_include_directories(rtype_server_tick_loop PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_tick_loop
  PUBLIC
    rtype_engine_time
    rtype_engine_fixed_timestep
    rtype_logging
  PRIVATE
    rtype_server_constants
    rtype_server_fine_timer_resolution
)

rtype_enable_warnings(rtype_server_tick_loop)
