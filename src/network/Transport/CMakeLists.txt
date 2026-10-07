add_library(rtype_network_transport INTERFACE)

target_include_directories(rtype_network_transport INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(UdpSocket)
