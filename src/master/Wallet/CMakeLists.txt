add_subdirectory(Contract)

if(NOT RTYPE_MODULE_WALLET)
  return()
endif()

add_subdirectory(Models)
add_subdirectory(Repositories)
add_subdirectory(Services)
add_subdirectory(Controllers)

add_library(rtype_master_wallet_module STATIC
  WalletModule.cpp
)

target_include_directories(rtype_master_wallet_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_wallet_module
  PUBLIC
    rtype_engine_commands
    rtype_engine_modules
    rtype_master_wallet_controllers
    rtype_master_wallet_services
  PRIVATE
    rtype_master_accounts_contract
    rtype_master_constants
    rtype_master_http_module
    rtype_master_storage_module
    rtype_master_wallet_contract
    rtype_network_master_economy
    rtype_network_master_routes
)

rtype_enable_warnings(rtype_master_wallet_module)
