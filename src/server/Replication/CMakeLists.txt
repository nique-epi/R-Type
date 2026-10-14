add_library(rtype_server_replication_constants INTERFACE)

target_include_directories(rtype_server_replication_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_constants
  INTERFACE
    rtype_engine_time
    rtype_network_protocol
)

add_subdirectory(WireEntityTypeMapping)
add_subdirectory(WorldStateSender)
add_subdirectory(GameEventEncoder)
add_subdirectory(GameEventBroadcaster)
