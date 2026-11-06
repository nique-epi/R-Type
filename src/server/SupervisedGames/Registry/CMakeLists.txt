add_library(rtype_server_supervised_games_registry STATIC
  GameLifecycle.cpp
  GameRegistry.cpp
  SeatReservations.cpp
)

target_include_directories(rtype_server_supervised_games_registry PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_registry
  PUBLIC
    rtype_engine_time
    rtype_network_game_status
    rtype_server_processes
  PRIVATE
    rtype_network_constants
    rtype_network_master_messages
    rtype_network_master_validators
    rtype_network_sequence_order
    rtype_server_exceptions
    rtype_server_games_module
    rtype_server_supervised_games_constants
)

rtype_enable_warnings(rtype_server_supervised_games_registry)
