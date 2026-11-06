add_library(rtype_master_probe_module STATIC
  MasterProbeModule.cpp
  ProbeSender.cpp
  ProbeThrottle.cpp
)

target_include_directories(rtype_master_probe_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_probe_module
  PUBLIC
    rtype_engine_modules
    rtype_logging
    rtype_network_context
    rtype_network_transport
    rtype_network_udp_socket
  PRIVATE
    rtype_master_constants
    rtype_network_constants
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_master_probe_module)
