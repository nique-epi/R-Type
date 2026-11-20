target_sources(client_screen_tests PRIVATE
  CasinoEntranceScreenFixture.cpp
  CasinoEntranceScreenTest.cpp
)
target_include_directories(client_screen_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
