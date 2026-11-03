add_library(rtype_client_server_leaderboard STATIC ServerLeaderboard.cpp)

target_include_directories(rtype_client_server_leaderboard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_server_leaderboard
  PUBLIC
    rtype_client_connection
    rtype_engine_time
    rtype_network_message_dispatcher
)

rtype_enable_warnings(rtype_client_server_leaderboard)
