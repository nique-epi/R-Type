add_library(rtype_network_leaderboard_answer STATIC LeaderboardAnswer.cpp)

target_include_directories(rtype_network_leaderboard_answer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_leaderboard_answer
  PUBLIC
    rtype_network_leaderboard_request
    rtype_network_protocol
  PRIVATE
    rtype_network_byte_reader
    rtype_network_byte_writer
    rtype_network_exceptions
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_network_leaderboard_answer)
