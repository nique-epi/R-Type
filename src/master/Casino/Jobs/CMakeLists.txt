add_library(rtype_master_casino_jobs STATIC CasinoRoundJob.cpp)

target_include_directories(rtype_master_casino_jobs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_casino_jobs
  PUBLIC
    rtype_logging
    rtype_master_casino_models
    rtype_master_casino_services
  PRIVATE
    rtype_concurrency_interruptible_sleep
    rtype_master_constants
)

rtype_enable_warnings(rtype_master_casino_jobs)
