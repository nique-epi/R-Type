target_sources(network_tests PRIVATE DatagramRelayTest.cpp DatagramRelayFixtures.cpp EndpointTextTest.cpp)

add_subdirectory(Conditions)
add_subdirectory(Simulator)
