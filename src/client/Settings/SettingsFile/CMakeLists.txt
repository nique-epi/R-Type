add_library(rtype_client_settings_file STATIC SettingsFile.cpp)

target_include_directories(rtype_client_settings_file PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings_file
  PUBLIC
    rtype_client_settings
    rtype_logging
  PRIVATE
    rtype_client_assets
)

rtype_enable_warnings(rtype_client_settings_file)
