add_library(rtype_server_supervised_games_stopping STATIC ChildrenStopper.cpp)

target_include_directories(rtype_server_supervised_games_stopping PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_supervised_games_stopping
  PUBLIC
    rtype_engine_time
    rtype_server_network_module
    rtype_server_supervised_games_registry
  PRIVATE
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_network_disconnection
    rtype_network_transport
    rtype_server_constants
    rtype_server_supervised_games_constants
)

rtype_enable_warnings(rtype_server_supervised_games_stopping)
