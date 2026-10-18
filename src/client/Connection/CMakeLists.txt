add_library(rtype_client_connection INTERFACE)

target_include_directories(rtype_client_connection INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection
  INTERFACE
    rtype_client_action_set
    rtype_engine_time
    rtype_network_connection_refused
    rtype_network_transport
)

add_subdirectory(NetworkThread)
add_subdirectory(PlayerInputPacer)
add_subdirectory(ServerConnection)
