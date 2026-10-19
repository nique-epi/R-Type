add_library(rtype_server_constants INTERFACE)

target_include_directories(rtype_server_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_constants INTERFACE rtype_engine_time)

add_subdirectory(Platform)
add_subdirectory(Application)
add_subdirectory(LaunchOptions)
add_subdirectory(Replication)
add_subdirectory(Instances)
add_subdirectory(Lobby)

add_executable(r-type_server main.cpp)

target_link_libraries(r-type_server
  PRIVATE
    rtype_logging
    rtype_network_constants
    rtype_server_application
    rtype_server_connection_acceptor
    rtype_server_constants
    rtype_server_game_event_broadcaster
    rtype_server_game_instance
    rtype_server_launch_options
    rtype_server_world_state_sender
)

rtype_enable_warnings(r-type_server)
