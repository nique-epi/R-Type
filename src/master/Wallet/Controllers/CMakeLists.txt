add_library(rtype_master_wallet_controllers STATIC GameTicketController.cpp WalletController.cpp)

target_include_directories(rtype_master_wallet_controllers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_wallet_controllers
  PUBLIC
    rtype_master_accounts_contract
    rtype_master_http_module
    rtype_master_wallet_services
  PRIVATE
    rtype_master_servers_controllers
    rtype_master_wallet_helpers
    rtype_network_master_economy
    rtype_network_master_routes
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_wallet_controllers)
