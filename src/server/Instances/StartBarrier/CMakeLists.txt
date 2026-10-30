add_library(rtype_server_start_barrier STATIC StartBarrier.cpp)

target_include_directories(rtype_server_start_barrier PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_start_barrier
  PUBLIC
    rtype_logging
    rtype_network_transport
    rtype_server_player_roster
  PRIVATE
    rtype_network_game_started
    rtype_server_instances
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_start_barrier)
