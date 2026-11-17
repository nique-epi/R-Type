add_library(rtype_master_wallet_repositories STATIC
  CreditedResultRepository.cpp
  GameTicketRepository.cpp
  OwnedLookRepository.cpp
  WalletMovementRepository.cpp
  WalletRepository.cpp
)

target_include_directories(rtype_master_wallet_repositories PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_wallet_repositories
  PUBLIC
    rtype_master_servers_models
    rtype_master_wallet_contract
    rtype_master_wallet_models
    rtype_storage
  PRIVATE
    rtype_master_servers_repositories
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_wallet_repositories)
