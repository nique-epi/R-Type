add_subdirectory(Setup)
add_subdirectory(Interfaces)

add_library(rtype_server_instances_context INTERFACE)

target_link_libraries(rtype_server_instances_context
  INTERFACE
    rtype_server_instances_context_interfaces
    rtype_server_instances_context_setup
)
