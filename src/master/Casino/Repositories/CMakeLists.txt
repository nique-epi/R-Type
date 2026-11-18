add_library(rtype_master_casino_repositories STATIC
  CasinoBetRepository.cpp
  CasinoBindings.cpp
  CasinoRoundRepository.cpp
  CasinoSeatRepository.cpp
)

target_include_directories(rtype_master_casino_repositories PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_casino_repositories
  PUBLIC
    rtype_master_casino_models
    rtype_storage
  PRIVATE
    rtype_master_servers_repositories
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_casino_repositories)
