add_library(rtype_client_update_release_constants INTERFACE)

target_include_directories(rtype_client_update_release_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Offer)
add_subdirectory(Verification)
add_subdirectory(Check)

add_library(rtype_client_update_release INTERFACE)

target_link_libraries(rtype_client_update_release
  INTERFACE
    rtype_client_update_release_check
    rtype_client_update_release_offer
    rtype_client_update_release_verification
)
