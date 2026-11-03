add_library(rtype_client_connection_leaderboard STATIC ServerLeaderboard.cpp)

target_include_directories(rtype_client_connection_leaderboard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_leaderboard
  PUBLIC
    rtype_client_session
    rtype_engine_time
    rtype_network_leaderboard_answer
    rtype_network_leaderboard_request
    rtype_network_lobby_state
    rtype_network_message_dispatcher
  PRIVATE
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_connection_leaderboard)
