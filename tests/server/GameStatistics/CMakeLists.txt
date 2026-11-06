# The plugin the tests load is the one the build puts next to the programs:
# PluginFiles reads its folder from RTYPE_TEST_PLUGIN_FOLDER.
target_sources(server_module_tests PRIVATE GameStatisticsFixtures.cpp ServerGameStatisticsFixture.cpp ServerGameStatisticsModuleTest.cpp ${RTYPE_TEST_SOURCE_DIR}/doubles/Plugins/PluginFiles.cpp)
target_include_directories(server_module_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${RTYPE_TEST_SOURCE_DIR}/doubles/Plugins)
target_compile_definitions(server_module_tests PRIVATE RTYPE_TEST_PLUGIN_FOLDER="$<TARGET_FILE_DIR:rtype_server_game_statistics_module>")

# A MODULE library is linked by nothing: the tests that load it wait for it.
add_dependencies(server_module_tests rtype_server_game_statistics_module)
