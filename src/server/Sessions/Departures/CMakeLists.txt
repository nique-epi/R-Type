add_library(rtype_server_departures STATIC Departures.cpp)

target_include_directories(rtype_server_departures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_departures
  PUBLIC
    rtype_engine_event_bus
    rtype_game_events
    rtype_logging
    rtype_network_transport
    rtype_server_player_roster
    rtype_server_sessions
)

rtype_enable_warnings(rtype_server_departures)
