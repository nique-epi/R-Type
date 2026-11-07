if(NOT RTYPE_MODULE_GAME_STATISTICS)
  return()
endif()

rtype_add_plugin(TARGET rtype_server_game_statistics_module NAME game-statistics
  SOURCES
    GameStatisticsPluginEntry.cpp
    GameTally.cpp
    ServerGameStatisticsModule.cpp
    StatisticsSheet.cpp
  LIBS
    rtype_server_games_module
    rtype_network_statistics_recording
)

target_include_directories(rtype_server_game_statistics_module PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
