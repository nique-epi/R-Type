add_library(rtype_client_update_releases STATIC
  ReleaseFeedSource.cpp
  ReleaseFiles.cpp
)

target_include_directories(rtype_client_update_releases PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_releases
  PUBLIC
    rtype_client_update_interfaces
    rtype_logging
    velopack::velopack
  PRIVATE
    httplib::httplib
    rtype_network_http_api
)

rtype_enable_warnings(rtype_client_update_releases)
