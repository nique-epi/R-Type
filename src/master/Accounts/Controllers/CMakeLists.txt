add_library(rtype_master_accounts_controllers STATIC
  AccountsController.cpp
  DeviceLinksController.cpp
)

target_include_directories(rtype_master_accounts_controllers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_accounts_controllers
  PUBLIC
    rtype_master_accounts_services
    rtype_master_http_module
  PRIVATE
    rtype_master_servers_controllers
    rtype_network_master_routes
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_accounts_controllers)
