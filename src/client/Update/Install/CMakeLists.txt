find_package(nlohmann_json CONFIG REQUIRED)

if(WIN32)
  set(RTYPE_UPDATE_PLATFORM_SOURCES ChildProcessWindows.cpp InstallPlaceWindows.cpp PackageInstallWindows.cpp)
elseif(APPLE)
  set(RTYPE_UPDATE_PLATFORM_SOURCES ChildProcessPosix.cpp InstallPlaceMacOs.cpp PackageInstallMacOs.cpp)
else()
  set(RTYPE_UPDATE_PLATFORM_SOURCES ChildProcessPosix.cpp InstallPlaceLinux.cpp PackageInstallLinux.cpp)
endif()

add_library(rtype_client_update_install STATIC
  ReleaseNotesFile.cpp
  UpdateRun.cpp
  ${RTYPE_UPDATE_PLATFORM_SOURCES}
)

target_include_directories(rtype_client_update_install PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rtype_client_update_install
  PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

target_link_libraries(rtype_client_update_install
  PUBLIC
    rtype_client_content_pack_download
    rtype_client_update_release
    rtype_logging
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_client_update_constants
    rtype_engine_paths
    rtype_network_byte_text
)

if(WIN32)
  target_link_libraries(rtype_client_update_install PRIVATE shell32)
endif()

rtype_enable_warnings(rtype_client_update_install)
