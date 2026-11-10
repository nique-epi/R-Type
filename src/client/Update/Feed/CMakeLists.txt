add_library(rtype_client_update_feed STATIC
  ReleaseFiles.cpp
  ReleaseFeedSource.cpp
)

target_include_directories(rtype_client_update_feed PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_feed
  PUBLIC
    rtype_logging
    velopack::velopack
  PRIVATE
    httplib::httplib
    rtype_client_update_interfaces
    rtype_network_http_api
)

rtype_enable_warnings(rtype_client_update_feed)
