add_library(rtype_client_settings INTERFACE)

target_include_directories(rtype_client_settings INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings
  INTERFACE
    rtype_client_key_bindings
    rtype_client_window
)

add_subdirectory(SettingsText)
add_subdirectory(SettingsFile)
add_subdirectory(SettingsService)
