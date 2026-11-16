add_subdirectory(Helpers)

add_library(rtype_master_wallet_services STATIC WalletService.cpp)

target_include_directories(rtype_master_wallet_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_wallet_services
  PUBLIC
    rtype_master_servers_models
    rtype_master_storage_module
    rtype_master_wallet_contract
    rtype_master_wallet_models
  PRIVATE
    rtype_master_wallet_helpers
    rtype_master_wallet_repositories
    rtype_network_master_economy
)

rtype_enable_warnings(rtype_master_wallet_services)
