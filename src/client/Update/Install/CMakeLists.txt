add_library(rtype_client_update_install_constants INTERFACE)

target_include_directories(rtype_client_update_install_constants INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Platform)
add_subdirectory(Notes)
add_subdirectory(Run)

add_library(rtype_client_update_install INTERFACE)

target_link_libraries(rtype_client_update_install
  INTERFACE
    rtype_client_update_install_constants
    rtype_client_update_install_notes
    rtype_client_update_install_platform
    rtype_client_update_install_run
)
