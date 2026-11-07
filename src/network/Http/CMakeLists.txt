add_subdirectory(Api)
add_subdirectory(Server)

add_library(rtype_network_http INTERFACE)

target_link_libraries(rtype_network_http
  INTERFACE
    rtype_network_http_api
    rtype_network_http_server
)
