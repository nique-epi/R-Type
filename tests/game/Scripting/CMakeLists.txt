rtype_add_test(NAME game_scripting_tests SOURCES BehaviourScriptFilesTest.cpp BehaviourScriptsFixtures.cpp BehaviourScriptsTest.cpp LuauBehaviourFixture.cpp LuauBehaviourTest.cpp LuauPreHitTest.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/RecordingEntityOrders.cpp LIBS rtype_game_scripting rtype_game_exceptions rtype_test_support GTest::gmock)
target_include_directories(game_scripting_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles)
target_compile_definitions(game_scripting_tests PRIVATE RTYPE_SOURCE_RESOURCES_FOLDER="${PROJECT_SOURCE_DIR}/resources")

add_subdirectory(Components)
add_subdirectory(Events)
add_subdirectory(Level)
