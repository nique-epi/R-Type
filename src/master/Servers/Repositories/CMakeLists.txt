add_library(rtype_master_servers_repositories STATIC
  HeartbeatSampleRepository.cpp
  ServerRepository.cpp
)

target_include_directories(rtype_master_servers_repositories PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_repositories
  PUBLIC
    rtype_master_servers_models
    rtype_master_storage_module
  PRIVATE
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_servers_repositories)
