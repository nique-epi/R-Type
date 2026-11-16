add_library(rtype_client_connection_casino INTERFACE)

target_include_directories(rtype_client_connection_casino INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_connection_casino
  INTERFACE
    rtype_network_master_casino_cloth
    rtype_network_master_casino_table
)
