add_subdirectory(Helpers)

add_library(rtype_master_accounts_services STATIC AccountsService.cpp)

target_include_directories(rtype_master_accounts_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_accounts_services
  PUBLIC
    rtype_logging
    rtype_master_accounts_models
    rtype_master_accounts_repositories
  PRIVATE
    rtype_master_accounts_helpers
    rtype_master_constants
    rtype_network_hashing
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_accounts_services)
