if(NOT RTYPE_MODULE_ACCOUNTS)
  return()
endif()

add_subdirectory(Models)
add_subdirectory(Repositories)
add_subdirectory(Services)
add_subdirectory(Controllers)

add_library(rtype_master_accounts_module STATIC
  AccountsModule.cpp
)

target_include_directories(rtype_master_accounts_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_accounts_module
  PUBLIC
    rtype_engine_commands
    rtype_engine_modules
    rtype_logging
    rtype_master_accounts_controllers
    rtype_master_http_module
  PRIVATE
    rtype_engine_exceptions
    rtype_master_accounts_helpers
    rtype_master_constants
    rtype_master_storage_module
    rtype_network_hashing
    rtype_network_master_routes
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_master_accounts_module)
