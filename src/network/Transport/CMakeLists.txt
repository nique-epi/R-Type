add_library(rtype_network_transport STATIC EndpointText.cpp)

target_include_directories(rtype_network_transport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_transport
  PUBLIC
    rtype_network_exceptions
  PRIVATE
    rtype_network_constants
)

rtype_enable_warnings(rtype_network_transport)

add_subdirectory(UdpSocket)
add_subdirectory(DatagramRelay)
add_subdirectory(Conditions)
add_subdirectory(Simulator)
