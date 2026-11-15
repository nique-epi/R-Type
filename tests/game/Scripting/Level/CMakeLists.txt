target_sources(game_scripting_tests PRIVATE
  LevelPlacementFilesFixture.cpp
  LevelPlacementFilesTest.cpp
  LuauLevelRunFixture.cpp
  LuauLevelRunTest.cpp
  LuauLevelsTest.cpp
  ${RTYPE_TEST_SOURCE_DIR}/game/Level/Ldtk/LdtkPlacementFixtures.cpp
)

target_include_directories(game_scripting_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/game/Level/Ldtk)

target_link_libraries(game_scripting_tests PRIVATE rtype_test_shipped_entity_sheets)
