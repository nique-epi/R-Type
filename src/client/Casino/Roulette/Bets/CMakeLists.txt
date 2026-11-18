add_library(rtype_client_casino_roulette_bets STATIC
  ChipsAtHand.cpp
  RouletteMemory.cpp
  RouletteSlip.cpp
)

target_include_directories(rtype_client_casino_roulette_bets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_roulette_bets
  PUBLIC
    rtype_client_connection_casino
    rtype_network_master_casino_cloth
)

rtype_enable_warnings(rtype_client_casino_roulette_bets)
