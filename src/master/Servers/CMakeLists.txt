find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_master_servers_module STATIC
  ServerRepository.cpp
  ServersController.cpp
  ServersModule.cpp
  ServersService.cpp
)

target_include_directories(rtype_master_servers_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_module
  PUBLIC
    rtype_engine_modules
    rtype_master_http_module
    rtype_master_server_keys_module
    rtype_master_storage_module
    rtype_network_master
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_master_constants
)

rtype_enable_warnings(rtype_master_servers_module)
