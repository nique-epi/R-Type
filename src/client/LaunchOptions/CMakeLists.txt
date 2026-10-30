add_library(rtype_client_launch_options STATIC LaunchOptions.cpp)

target_include_directories(rtype_client_launch_options PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_launch_options
  PUBLIC
    rtype_client_audio_interfaces
    rtype_client_exceptions
    rtype_engine_launch_arguments
    rtype_network_exceptions
    rtype_network_transport
  PRIVATE
    rtype_client_constants
    rtype_logging
    rtype_network_constants
    rtype_network_protocol
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_client_launch_options)
