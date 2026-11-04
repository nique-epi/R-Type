add_library(rtype_tools_malformed_datagrams_flood STATIC
  DatagramFlood.cpp
  LoopbackPeer.cpp
  SessionOpening.cpp
)

target_include_directories(rtype_tools_malformed_datagrams_flood PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_malformed_datagrams_flood
  PUBLIC
    rtype_network_asio
    rtype_tools_malformed_datagrams_builder
    rtype_tools_malformed_datagrams_constants
)

rtype_enable_warnings(rtype_tools_malformed_datagrams_flood)
