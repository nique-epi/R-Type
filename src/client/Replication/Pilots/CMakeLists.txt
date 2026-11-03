add_library(rtype_client_pilots STATIC
  FinalRanking.cpp
  ScoreBoard.cpp
)

target_include_directories(rtype_client_pilots PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_pilots
  PUBLIC
    rtype_client_entities
    rtype_network_game_ended
    rtype_network_lobby_state
    rtype_network_player_left
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_pilots)
