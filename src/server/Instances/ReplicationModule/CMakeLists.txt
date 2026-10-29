add_library(rtype_server_replication_module STATIC ReplicationModule.cpp)

target_include_directories(rtype_server_replication_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_module
  PUBLIC
    rtype_engine_install_context
    rtype_server_game_event_broadcaster
    rtype_server_game_replication
    rtype_server_instances
  PRIVATE
    rtype_server_session_datagram
)

rtype_enable_warnings(rtype_server_replication_module)
