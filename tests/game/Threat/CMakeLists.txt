target_sources(game_tests PRIVATE FirepowerTest.cpp FirepowerFixtures.cpp ThreatRankTest.cpp ThreatRankFixtures.cpp)
target_include_directories(game_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/game/EntityTypes)
target_link_libraries(game_tests PRIVATE rtype_game_threat)
