add_subdirectory(Cloth)
add_subdirectory(Draw)
add_subdirectory(Table)

add_library(rtype_network_master_casino INTERFACE)

target_link_libraries(rtype_network_master_casino
  INTERFACE
    rtype_network_master_casino_cloth
    rtype_network_master_casino_draw
    rtype_network_master_casino_table
)
