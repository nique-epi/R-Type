add_library(rtype_network_world_state_delta STATIC WorldStateDelta.cpp)

target_include_directories(rtype_network_world_state_delta PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_world_state_delta
  PUBLIC
    rtype_network_protocol
  PRIVATE
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_network_world_state_delta)
