add_library(rtype_client_account_wallet STATIC
  AccountWallet.cpp
  WalletPurse.cpp
)

target_include_directories(rtype_client_account_wallet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_account_wallet
  PUBLIC
    rtype_client_account_session
    rtype_client_connection_account
    rtype_client_economy
    rtype_client_settings_values
  PRIVATE
    rtype_identifiers
    rtype_network_master_economy
)

rtype_enable_warnings(rtype_client_account_wallet)
