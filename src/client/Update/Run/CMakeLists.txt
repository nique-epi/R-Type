find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_client_update_run STATIC
  NoteLines.cpp
  ReleaseNotesFile.cpp
  UpdateCheck.cpp
  UpdateRun.cpp
)

target_include_directories(rtype_client_update_run PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_update_run
  PUBLIC
    rtype_client_update_interfaces
    rtype_logging
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_client_exceptions
)

rtype_enable_warnings(rtype_client_update_run)
