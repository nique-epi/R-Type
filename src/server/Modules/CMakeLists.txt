add_library(rtype_server_modules STATIC
  FarewellSender.cpp
  Games.cpp
  ServerGamesModule.cpp
  ServerJournalModule.cpp
  ServerLobbyModule.cpp
  ServerModules.cpp
  ServerNetworkModule.cpp
  TickDatagrams.cpp
)

target_include_directories(rtype_server_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_modules
  PUBLIC
    rtype_engine_application
    rtype_engine_event_bus
    rtype_engine_modules
    rtype_engine_system_clock
    rtype_engine_time
    rtype_engine_timer_scheduler
    rtype_logging
    rtype_network_transport
    rtype_server_application
    rtype_server_application_interfaces
    rtype_server_connection_acceptor
    rtype_server_exceptions
    rtype_server_game_event_broadcaster
    rtype_server_game_instance
    rtype_server_launch_options
    rtype_server_stop_request
  PRIVATE
    rtype_network_constants
    rtype_network_disconnection
    rtype_server_constants
    rtype_server_replication_constants
    rtype_server_session_datagram
    rtype_server_wave_catalog
)

rtype_enable_warnings(rtype_server_modules)
