add_library(rtype_client_settings STATIC SettingsText.cpp)

target_include_directories(rtype_client_settings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings
  PUBLIC
    rtype_client_key_bindings
    rtype_client_window
  PRIVATE
    rtype_client_key_names
)

rtype_enable_warnings(rtype_client_settings)

add_subdirectory(SettingsFile)
