add_library(rtype_master_launch_options STATIC MasterLaunchOptions.cpp)

target_include_directories(rtype_master_launch_options PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_launch_options
  PUBLIC
    rtype_engine_commands
    rtype_engine_launch_arguments
    rtype_logging
    rtype_network_exceptions
  PRIVATE
    rtype_engine_paths
    rtype_master_constants
    rtype_network_constants
    rtype_network_master
    rtype_network_transport
)

rtype_enable_warnings(rtype_master_launch_options)
