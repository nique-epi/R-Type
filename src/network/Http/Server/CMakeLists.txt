find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_network_http_server STATIC ClientAddress.cpp HttpServer.cpp)

target_include_directories(rtype_network_http_server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_network_http_server
  PUBLIC
    rtype_logging
    rtype_network_http_api
    rtype_network_transport
  PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
    rtype_network_constants
    rtype_network_exceptions
)

rtype_enable_warnings(rtype_network_http_server)
