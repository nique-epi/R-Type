add_library(rtype_engine_server_clock STATIC ServerClock.cpp)

target_include_directories(rtype_engine_server_clock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_server_clock
  PUBLIC
    rtype_engine_time
)

rtype_enable_warnings(rtype_engine_server_clock)
