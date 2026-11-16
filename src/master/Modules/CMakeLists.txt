add_library(rtype_master_modules STATIC MasterModules.cpp)

target_include_directories(rtype_master_modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_modules
  PUBLIC
    rtype_engine_modules
    rtype_logging
    rtype_master_http_module
  PRIVATE
    rtype_engine_paths
    rtype_master_leaderboard_module
    rtype_master_logger_module
    rtype_master_probe_module
    rtype_master_servers_module
    rtype_master_storage_module
)

if(RTYPE_MODULE_ACCOUNTS)
  target_link_libraries(rtype_master_modules PRIVATE rtype_master_accounts_module)
endif()

if(RTYPE_MODULE_WALLET)
  target_link_libraries(rtype_master_modules PRIVATE rtype_master_wallet_module)
endif()

rtype_enable_warnings(rtype_master_modules)
