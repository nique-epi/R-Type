find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_client_update_install_notes STATIC
  ReleaseNotesFile.cpp
)

target_include_directories(rtype_client_update_install_notes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_install_notes
  PUBLIC
    rtype_client_update_install_constants
  PRIVATE
    nlohmann_json::nlohmann_json
)

rtype_enable_warnings(rtype_client_update_install_notes)
