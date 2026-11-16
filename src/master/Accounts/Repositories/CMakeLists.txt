add_library(rtype_master_accounts_repositories STATIC
  UserRepository.cpp
  UserRow.cpp
  SessionRepository.cpp
  SignInFailureRepository.cpp
)

target_include_directories(rtype_master_accounts_repositories PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_accounts_repositories
  PUBLIC
    rtype_master_accounts_models
    rtype_master_servers_models
    rtype_master_storage_module
  PRIVATE
    rtype_master_servers_repositories
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_accounts_repositories)
