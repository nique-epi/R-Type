add_library(rtype_server_world_state_sender STATIC WorldStateSender.cpp)

target_include_directories(rtype_server_world_state_sender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_world_state_sender
  PUBLIC
    rtype_engine
    rtype_game
    rtype_network_transport
    rtype_network_world_state_codec
  PRIVATE
    rtype_server_constants
)

rtype_enable_warnings(rtype_server_world_state_sender)
