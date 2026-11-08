add_library(rtype_server_admin_console_commands STATIC AdminConsoleCommands.cpp BanCommands.cpp BanWords.cpp CommandWords.cpp ConsoleTexts.cpp)
target_include_directories(rtype_server_admin_console_commands PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_server_admin_console_commands
  PUBLIC
    rtype_engine_commands
    rtype_network_master_messages
    rtype_network_statistics
    rtype_server_admin_console_output
    rtype_server_ban_book_module
    rtype_server_network_module
    rtype_server_supervised_games_module
  PRIVATE
    rtype_network_exceptions
    rtype_network_expulsion_order
    rtype_network_transport
    rtype_network_wire_types
    rtype_server_admin_console_constants
    rtype_server_constants
    rtype_server_supervised_games_stopping
)
rtype_enable_warnings(rtype_server_admin_console_commands)
