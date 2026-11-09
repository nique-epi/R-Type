add_library(rtype_client_update_release_check STATIC
  ReleaseCheck.cpp
  ReleaseFetch.cpp
)

target_include_directories(rtype_client_update_release_check PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_release_check
  PUBLIC
    rtype_client_update_release_offer
    rtype_client_update_release_verification
    rtype_logging
  PRIVATE
    httplib::httplib
    rtype_client_update_constants
    rtype_client_update_release_constants
    rtype_network_http_api
)

rtype_enable_warnings(rtype_client_update_release_check)
