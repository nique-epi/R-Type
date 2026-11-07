add_library(rtype_server_admin_console_constants INTERFACE)
target_include_directories(rtype_server_admin_console_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Input)
add_subdirectory(Output)
add_subdirectory(Commands)

add_library(rtype_server_admin_console_module STATIC ServerAdminConsoleModule.cpp)
target_include_directories(rtype_server_admin_console_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rtype_server_admin_console_module
  PUBLIC
    rtype_concurrency_bounded_queue
    rtype_engine_commands
    rtype_engine_modules
    rtype_server_admin_console_commands
    rtype_server_admin_console_constants
    rtype_server_admin_console_input
    rtype_server_admin_console_output
  PRIVATE
    rtype_engine_time
    rtype_server_constants
    rtype_server_network_module
    rtype_server_supervised_games_module
)
rtype_enable_warnings(rtype_server_admin_console_module)
