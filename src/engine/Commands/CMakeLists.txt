add_subdirectory(Interpreter)
add_subdirectory(Settings)
add_subdirectory(Configuration)

add_library(rtype_engine_commands INTERFACE)

target_link_libraries(rtype_engine_commands
  INTERFACE
    rtype_engine_command_interpreter
    rtype_engine_command_settings
    rtype_engine_command_configuration
)
