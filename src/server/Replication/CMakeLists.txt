add_subdirectory(Wire)
add_subdirectory(Announcements)

add_library(rtype_server_replication_module STATIC GameReplication.cpp ReplicationModule.cpp)

target_include_directories(rtype_server_replication_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_replication_module
  PUBLIC
    rtype_engine_install_context
    rtype_server_instances
    rtype_server_replication_announcements
    rtype_server_replication_wire
    rtype_server_sessions_roster
)

rtype_enable_warnings(rtype_server_replication_module)
