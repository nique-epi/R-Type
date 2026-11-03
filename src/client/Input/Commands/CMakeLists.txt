add_library(rtype_client_input_commands STATIC
  ActionSet.cpp
  PlayerCommands.cpp
  PlayerInputPacer.cpp
)

target_include_directories(rtype_client_input_commands PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_input_commands
  PUBLIC
    SFML::Window
    rtype_client_input_keys
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_input_commands)
