add_library(rtype_master_servers_helpers STATIC
  ServerKey.cpp
  ServerStatusRule.cpp
)

target_include_directories(rtype_master_servers_helpers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_helpers
  PUBLIC
    rtype_master_servers_models
  PRIVATE
    rtype_network_hashing
    rtype_engine_time
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_servers_helpers)
