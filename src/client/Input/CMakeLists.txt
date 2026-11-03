add_subdirectory(Keys)
add_subdirectory(Commands)

add_library(rtype_client_input INTERFACE)

target_link_libraries(rtype_client_input
  INTERFACE
    rtype_client_input_commands
    rtype_client_input_keys
)
