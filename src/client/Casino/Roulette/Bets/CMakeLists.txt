add_library(rtype_client_casino_roulette_bets STATIC
  RouletteMemory.cpp
  RouletteSlip.cpp
)

target_include_directories(rtype_client_casino_roulette_bets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_roulette_bets
  PUBLIC
    rtype_network_roulette_bets
    rtype_network_roulette_round
  PRIVATE
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_casino_roulette_bets)
