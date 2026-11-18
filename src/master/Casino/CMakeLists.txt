if(NOT RTYPE_MODULE_CASINO)
  return()
endif()

add_subdirectory(Models)
add_subdirectory(Repositories)
add_subdirectory(Services)
add_subdirectory(Controllers)
add_subdirectory(Jobs)

add_library(rtype_master_casino_module STATIC
  CasinoModule.cpp
)

target_include_directories(rtype_master_casino_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_casino_module
  PUBLIC
    rtype_engine_commands
    rtype_engine_modules
    rtype_master_casino_controllers
    rtype_master_casino_jobs
    rtype_master_casino_services
    rtype_network_master_casino_draw
  PRIVATE
    rtype_master_accounts_contract
    rtype_master_casino_helpers
    rtype_master_constants
    rtype_master_http_module
    rtype_master_storage_module
    rtype_master_wallet_contract
    rtype_network_master_casino_table
    rtype_network_master_routes
)

rtype_enable_warnings(rtype_master_casino_module)
