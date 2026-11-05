add_library(rtype_server_launch_options STATIC LaunchOptions.cpp ServerModeOption.cpp)

target_include_directories(rtype_server_launch_options PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_launch_options
  PUBLIC
    rtype_engine_commands
    rtype_engine_launch_arguments
    rtype_logging
    rtype_network_conditions
    rtype_network_exceptions
    rtype_network_transport
  PRIVATE
    rtype_engine_paths
    rtype_network_constants
    rtype_network_protocol
    rtype_server_constants
    rtype_server_exceptions
)

rtype_enable_warnings(rtype_server_launch_options)
