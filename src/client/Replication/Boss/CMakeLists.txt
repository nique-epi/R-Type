add_library(rtype_client_boss STATIC BossTracker.cpp)

target_include_directories(rtype_client_boss PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_boss
  PUBLIC
    rtype_engine_time
    rtype_identifiers
    rtype_network_boss_status
)

rtype_enable_warnings(rtype_client_boss)
