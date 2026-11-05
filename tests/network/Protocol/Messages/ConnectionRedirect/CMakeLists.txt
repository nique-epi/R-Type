target_sources(network_tests PRIVATE ConnectionRedirectTest.cpp)

target_include_directories(network_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
