target_sources(server_module_tests PRIVATE GameStatisticsFixtures.cpp ServerGameStatisticsFixture.cpp ServerGameStatisticsModuleTest.cpp)
target_include_directories(server_module_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(server_module_tests PRIVATE RTYPE_GAME_STATISTICS_PLUGIN_FOLDER="$<TARGET_FILE_DIR:rtype_server_game_statistics_module>")

# A MODULE library is linked by nothing: the tests that load it wait for it.
add_dependencies(server_module_tests rtype_server_game_statistics_module)
