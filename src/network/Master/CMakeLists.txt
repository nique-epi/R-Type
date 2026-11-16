add_subdirectory(Routes)
add_subdirectory(Messages)
add_subdirectory(Validators)
add_subdirectory(Client)
add_subdirectory(Casino)

add_library(rtype_network_master INTERFACE)

target_link_libraries(rtype_network_master
  INTERFACE
    rtype_network_master_casino
    rtype_network_master_messages
    rtype_network_master_routes
    rtype_network_master_validators
)
