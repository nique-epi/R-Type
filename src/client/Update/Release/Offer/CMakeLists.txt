add_library(rtype_client_update_release_offer STATIC
  ReleaseVersion.cpp
)

target_include_directories(rtype_client_update_release_offer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_release_offer
  PUBLIC
    rtype_client_update_release_constants
)

rtype_enable_warnings(rtype_client_update_release_offer)
