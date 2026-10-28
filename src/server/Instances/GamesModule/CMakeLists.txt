add_library(rtype_server_games_module STATIC GamesModule.cpp)

target_include_directories(rtype_server_games_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_games_module
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_modules
    rtype_engine_time
    rtype_engine_timer_scheduler
    rtype_logging
    rtype_server_application
    rtype_server_application_interfaces
    rtype_server_game_event_broadcaster
    rtype_server_game_list
    rtype_server_instances
  PRIVATE
    rtype_network_disconnection
    rtype_server_constants
    rtype_server_replication_constants
    rtype_server_session_datagram
    rtype_server_wave_catalog
)

rtype_enable_warnings(rtype_server_games_module)
