add_library(rtype_client_casino_roulette_cloth STATIC
  ClothBets.cpp
  ClothGeometry.cpp
  ClothMoves.cpp
)

target_include_directories(rtype_client_casino_roulette_cloth PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_roulette_cloth
  PUBLIC
    SFML::Graphics
    rtype_network_protocol
    rtype_network_roulette_bets
  PRIVATE
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_casino_roulette_cloth)
