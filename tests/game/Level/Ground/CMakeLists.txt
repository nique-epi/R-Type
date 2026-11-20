target_sources(game_tests PRIVATE GroundLookupFixtures.cpp GroundLookupTest.cpp
                                  GroundReadingTest.cpp)
target_include_directories(game_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(game_tests PRIVATE rtype_game_level_ground)
