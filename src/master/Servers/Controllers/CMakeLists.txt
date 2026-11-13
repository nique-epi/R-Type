find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_master_servers_controllers STATIC
  RouteAnswers.cpp
  ServersController.cpp
)

target_include_directories(rtype_master_servers_controllers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_controllers
  PUBLIC
    rtype_engine_commands
    rtype_master_http_module
    rtype_master_probe_module
    rtype_master_servers_services
    rtype_network_exceptions
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_master_servers_helpers
    rtype_network_master_routes
    rtype_network_master_validators
    rtype_network_transport
)

rtype_enable_warnings(rtype_master_servers_controllers)
