add_library(rtype_client_settings_storage STATIC
  SettingsFile.cpp
  SettingsText.cpp
)

target_include_directories(rtype_client_settings_storage PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings_storage
  PUBLIC
    rtype_client_settings_values
    rtype_logging
  PRIVATE
    SFML::Window
    rtype_client_input_keys
)

rtype_enable_warnings(rtype_client_settings_storage)
