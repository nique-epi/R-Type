add_library(rtype_network_simulator STATIC
  DatagramsInFlight.cpp
  DelayedDelivery.cpp
  SimulatedNetworkSocket.cpp
  DatagramSocketOpening.cpp
)

target_include_directories(rtype_network_simulator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_simulator
  PUBLIC
    rtype_network_conditions
    rtype_network_context
    rtype_network_transport
    rtype_network_udp_socket
  PRIVATE
    rtype_logging
    rtype_network_asio
    rtype_network_constants
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_simulator)
