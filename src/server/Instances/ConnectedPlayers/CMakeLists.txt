add_library(rtype_server_connected_players STATIC ConnectedPlayers.cpp)

target_include_directories(rtype_server_connected_players PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_connected_players
  PUBLIC
    rtype_network_protocol
  PRIVATE
    rtype_server_instances
)

rtype_enable_warnings(rtype_server_connected_players)
