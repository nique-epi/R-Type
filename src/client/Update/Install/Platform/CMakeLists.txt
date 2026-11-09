if(WIN32)
  set(RTYPE_UPDATE_PLATFORM_SOURCES ChildProcessWindows.cpp InstallPlaceWindows.cpp PackageInstallWindows.cpp)
elseif(APPLE)
  set(RTYPE_UPDATE_PLATFORM_SOURCES ChildProcessPosix.cpp InstallPlaceMacOs.cpp PackageInstallMacOs.cpp)
else()
  set(RTYPE_UPDATE_PLATFORM_SOURCES ChildProcessPosix.cpp InstallPlaceLinux.cpp PackageInstallLinux.cpp)
endif()

add_library(rtype_client_update_install_platform STATIC
  ${RTYPE_UPDATE_PLATFORM_SOURCES}
)

target_include_directories(rtype_client_update_install_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rtype_client_update_install_platform
  PRIVATE
    $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
    $<$<PLATFORM_ID:Windows>:NOMINMAX>
)

target_link_libraries(rtype_client_update_install_platform
  PUBLIC
    rtype_client_update_install_constants
  PRIVATE
    rtype_engine_paths
)

if(WIN32)
  target_link_libraries(rtype_client_update_install_platform PRIVATE shell32)
endif()

rtype_enable_warnings(rtype_client_update_install_platform)
