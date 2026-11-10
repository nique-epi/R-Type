add_library(rtype_client_update_updater STATIC VelopackUpdater.cpp)

target_include_directories(rtype_client_update_updater PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_updater
  PUBLIC
    rtype_client_update_feed
    rtype_client_update_interfaces
    velopack::velopack
  PRIVATE
    rtype_client_exceptions
)

rtype_enable_warnings(rtype_client_update_updater)
