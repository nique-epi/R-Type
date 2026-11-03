add_subdirectory(Roster)
add_subdirectory(Tracking)

add_library(rtype_server_sessions STATIC GameSessions.cpp)

target_include_directories(rtype_server_sessions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_sessions
  PUBLIC
    rtype_engine_event_bus
    rtype_network_leaderboard_request
    rtype_network_message_dispatcher
    rtype_network_transport
    rtype_server_sessions_roster
    rtype_server_sessions_tracking
  PRIVATE
    rtype_network_disconnection
    rtype_network_heartbeat
)

rtype_enable_warnings(rtype_server_sessions)
