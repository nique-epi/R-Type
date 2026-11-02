add_library(rtype_client_settings_service STATIC SettingsService.cpp)

target_include_directories(rtype_client_settings_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings_service
  PUBLIC
    rtype_client_settings
)

rtype_enable_warnings(rtype_client_settings_service)
