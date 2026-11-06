add_library(rtype_server_supervised_games_status STATIC GameStatusReading.cpp)

target_include_directories(rtype_server_supervised_games_status PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_status
  PUBLIC
    rtype_network_game_status
    rtype_network_transport
  PRIVATE
    rtype_network_datagram_codec
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_server_supervised_games_status)
