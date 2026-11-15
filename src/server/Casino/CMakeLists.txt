if(NOT RTYPE_MODULE_CASINO)
  return()
endif()

add_library(rtype_server_casino_module STATIC
  CasinoModule.cpp
  RouletteRoundAnnouncer.cpp
  RouletteRules.cpp
  RouletteTable.cpp
  SystemRouletteDraw.cpp
)

target_include_directories(rtype_server_casino_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_casino_module
  PUBLIC
    rtype_engine_event_bus
    rtype_engine_install_context
    rtype_logging
    rtype_network_casino_place
    rtype_network_roulette_bets
    rtype_network_roulette_round
    rtype_server_instances
  PRIVATE
    rtype_network_protocol
    rtype_server_sessions_roster
)

rtype_enable_warnings(rtype_server_casino_module)
