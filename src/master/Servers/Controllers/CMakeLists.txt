find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_master_servers_controllers STATIC
  ServersController.cpp
)

target_include_directories(rtype_master_servers_controllers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_controllers
  PUBLIC
    rtype_master_http_module
    rtype_master_servers_services
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_network_exceptions
    rtype_network_master_routes
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_servers_controllers)
