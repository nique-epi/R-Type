add_library(rtype_client_update_install_run STATIC
  UpdateRun.cpp
)

target_include_directories(rtype_client_update_install_run PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_install_run
  PUBLIC
    rtype_client_content_pack_download
    rtype_client_update_install_notes
    rtype_client_update_install_platform
    rtype_client_update_release
    rtype_logging
  PRIVATE
    rtype_client_update_constants
    rtype_network_byte_text
)

rtype_enable_warnings(rtype_client_update_install_run)
