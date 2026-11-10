add_library(rtype_client_update_velopack STATIC
  StartupHooks.cpp
  VelopackUpdater.cpp
)

target_include_directories(rtype_client_update_velopack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_velopack
  PUBLIC
    rtype_client_update_interfaces
    rtype_client_update_releases
    velopack::velopack
  PRIVATE
    rtype_client_exceptions
)

rtype_enable_warnings(rtype_client_update_velopack)
