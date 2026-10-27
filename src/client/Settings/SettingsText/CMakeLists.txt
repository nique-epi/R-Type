add_library(rtype_client_settings_text STATIC SettingsText.cpp)

target_include_directories(rtype_client_settings_text PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings_text
  PUBLIC
    rtype_client_settings
  PRIVATE
    rtype_client_key_names
)

rtype_enable_warnings(rtype_client_settings_text)
