add_library(rtype_server_server_link STATIC ServerLink.cpp)

target_include_directories(rtype_server_server_link PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_server_link
  PUBLIC
    rtype_network_transport
    rtype_server_application
    rtype_server_application_interfaces
  PRIVATE
    rtype_server_exceptions
)

rtype_enable_warnings(rtype_server_server_link)
