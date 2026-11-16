add_subdirectory(Helpers)

add_library(rtype_master_casino_services STATIC
  CasinoService.cpp
  RouletteDealer.cpp
)

target_include_directories(rtype_master_casino_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_casino_services
  PUBLIC
    rtype_master_casino_models
    rtype_master_storage_module
    rtype_master_wallet_contract
    rtype_network_master_casino_draw
  PRIVATE
    rtype_master_casino_helpers
    rtype_master_casino_repositories
    rtype_master_exceptions
    rtype_network_master_casino_cloth
)

rtype_enable_warnings(rtype_master_casino_services)
