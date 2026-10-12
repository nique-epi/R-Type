add_library(rtype_client_player_commands STATIC PlayerCommands.cpp)

target_include_directories(rtype_client_player_commands PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_player_commands
  PUBLIC
    SFML::Window
    rtype_client_action_set
    rtype_client_key_bindings
)

rtype_enable_warnings(rtype_client_player_commands)
