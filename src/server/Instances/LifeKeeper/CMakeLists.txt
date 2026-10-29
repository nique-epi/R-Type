add_library(rtype_server_life_keeper STATIC LifeKeeper.cpp)

target_include_directories(rtype_server_life_keeper PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_life_keeper
  PUBLIC
    rtype_engine_event_bus
    rtype_game
    rtype_logging
    rtype_server_instances
    rtype_server_player_roster
  PRIVATE
    rtype_identifiers
)

rtype_enable_warnings(rtype_server_life_keeper)
