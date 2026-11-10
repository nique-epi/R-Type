add_subdirectory(UpdatePanel)
add_subdirectory(WhatsNew)

add_library(rtype_client_update_screens INTERFACE)

target_link_libraries(rtype_client_update_screens
  INTERFACE
    rtype_client_update_panel
    rtype_client_update_whats_new
)
