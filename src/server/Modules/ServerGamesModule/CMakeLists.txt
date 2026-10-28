add_library(rtype_server_games_module STATIC ServerGamesModule.cpp)

target_include_directories(rtype_server_games_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_games_module
  PUBLIC
    rtype_engine_application
    rtype_engine_event_bus
    rtype_engine_timer_scheduler
    rtype_logging
    rtype_server_application_interfaces
    rtype_server_game_event_broadcaster
    rtype_server_games
    rtype_server_tick_datagrams
  PRIVATE
    rtype_server_constants
    rtype_server_farewell_sender
    rtype_server_module_constants
    rtype_server_wave_catalog
)

rtype_enable_warnings(rtype_server_games_module)
