add_library(rtype_server_score_keeper STATIC ScoreKeeper.cpp)

target_include_directories(rtype_server_score_keeper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_score_keeper
  PUBLIC
    rtype_engine_event_bus
    rtype_game_events
    rtype_server_player_roster
)

rtype_enable_warnings(rtype_server_score_keeper)
