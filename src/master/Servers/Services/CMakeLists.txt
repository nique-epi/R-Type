find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(Helpers)

add_library(rtype_master_servers_services STATIC
  ServersService.cpp
)

target_include_directories(rtype_master_servers_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_services
  PUBLIC
    rtype_master_servers_models
    rtype_master_servers_repositories
    rtype_network_master_messages
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_master_servers_helpers
    rtype_network_exceptions
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_servers_services)
