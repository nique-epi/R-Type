add_subdirectory(Cloth)
add_subdirectory(Bets)
add_subdirectory(Wheel)

add_library(rtype_client_casino_roulette INTERFACE)

target_link_libraries(rtype_client_casino_roulette
  INTERFACE
    rtype_client_casino_roulette_bets
    rtype_client_casino_roulette_cloth
    rtype_client_casino_roulette_wheel
)
