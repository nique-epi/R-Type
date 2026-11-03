add_library(rtype_server_network_module STATIC
  DatagramSorter.cpp
  ServerApplication.cpp
  ServerNetworkModule.cpp
  TickDatagrams.cpp
  TickLoop.cpp
)

target_include_directories(rtype_server_network_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_network_module
  PUBLIC
    rtype_concurrency_bounded_queue
    rtype_engine_application
    rtype_engine_fixed_timestep
    rtype_engine_system_clock
    rtype_engine_time
    rtype_logging
    rtype_network_context
    rtype_network_datagram_relay
    rtype_network_protocol
    rtype_network_simulator
    rtype_network_transport
    rtype_server_platform
  PRIVATE
    rtype_network_constants
    rtype_network_datagram_codec
    rtype_server_constants
    rtype_server_exceptions
)

rtype_enable_warnings(rtype_server_network_module)
