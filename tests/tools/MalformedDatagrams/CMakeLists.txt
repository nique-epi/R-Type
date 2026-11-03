add_library(rtype_tools_malformed_datagrams STATIC
  DatagramFlood.cpp
  FloodLaunchOptions.cpp
  LoopbackPeer.cpp
  MalformedDatagramBuilder.cpp
  SessionOpening.cpp
)

target_include_directories(rtype_tools_malformed_datagrams PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_tools_malformed_datagrams
  PUBLIC
    rtype_engine_launch_arguments
    rtype_network
    rtype_network_asio
)

rtype_enable_warnings(rtype_tools_malformed_datagrams)

add_executable(malformed_datagrams main.cpp)

target_link_libraries(malformed_datagrams PRIVATE rtype_tools_malformed_datagrams)

set_target_properties(malformed_datagrams PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

rtype_enable_warnings(malformed_datagrams)

rtype_add_test(NAME malformed_datagram_tests SOURCES MalformedDatagramBuilderTest.cpp MalformedDatagramBuilderFixture.cpp LIBS rtype_tools_malformed_datagrams)

target_include_directories(malformed_datagram_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/network/Protocol)
