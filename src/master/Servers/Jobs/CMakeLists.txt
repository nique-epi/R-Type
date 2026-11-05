add_library(rtype_master_servers_jobs STATIC
  ServerStatusJob.cpp
)

target_include_directories(rtype_master_servers_jobs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_servers_jobs
  PUBLIC
    rtype_logging
    rtype_master_servers_services
  PRIVATE
    rtype_concurrency_interruptible_sleep
    rtype_master_constants
    rtype_network_master_validators
)

rtype_enable_warnings(rtype_master_servers_jobs)
