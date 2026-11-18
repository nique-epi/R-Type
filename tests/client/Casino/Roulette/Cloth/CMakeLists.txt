target_sources(client_screen_tests PRIVATE
  ClothBetsFixtures.cpp
  ClothBetsTest.cpp
  ClothGeometryTest.cpp
  ClothMovesTest.cpp
)
target_include_directories(client_screen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(client_screen_tests PRIVATE rtype_client_casino_roulette_cloth)
