add_library(rtype_master_accounts_helpers STATIC
  DeviceLinkSecret.cpp
  PasswordKeyText.cpp
  SessionToken.cpp
)

target_include_directories(rtype_master_accounts_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_accounts_helpers
  PUBLIC
    rtype_network_hashing
  PRIVATE
    rtype_master_constants
    rtype_master_exceptions
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_accounts_helpers)
