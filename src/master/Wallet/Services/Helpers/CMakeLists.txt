add_library(rtype_master_wallet_helpers STATIC WalletBalances.cpp)

target_include_directories(rtype_master_wallet_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_wallet_helpers
  PUBLIC
    rtype_network_master_messages
  PRIVATE
    rtype_network_master_economy
)

rtype_enable_warnings(rtype_master_wallet_helpers)
