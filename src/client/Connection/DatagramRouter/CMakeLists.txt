add_library(rtype_client_datagram_router STATIC DatagramRouter.cpp)

target_include_directories(rtype_client_datagram_router PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_datagram_router
  PUBLIC
    rtype_client_connection
    rtype_client_datagram_route
    rtype_network_transport
)

rtype_enable_warnings(rtype_client_datagram_router)
