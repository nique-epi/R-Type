add_library(rtype_server_game_instance STATIC GameInstance.cpp)

target_include_directories(rtype_server_game_instance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_game_instance
  PUBLIC
    rtype_engine_event_bus
    rtype_game
    rtype_network_transport
    rtype_server_instances
    rtype_server_game_flow
    rtype_server_game_sessions
    rtype_server_player_roster
    rtype_server_score_keeper
  PRIVATE
    rtype_server_ship_input
)

rtype_enable_warnings(rtype_server_game_instance)
