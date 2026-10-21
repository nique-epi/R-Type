add_library(rtype_client_score_board STATIC ScoreBoard.cpp)

target_include_directories(rtype_client_score_board PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_score_board
  PUBLIC
    rtype_client_replication_interfaces
    rtype_client_world_state_filter
    rtype_network_lobby_state
    rtype_network_player_left
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_score_board)
