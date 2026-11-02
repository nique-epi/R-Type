add_library(rtype_client_settings_module STATIC ClientSettingsModule.cpp)

target_include_directories(rtype_client_settings_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings_module
  PUBLIC
    rtype_client_settings
    rtype_client_settings_file
    rtype_client_settings_service
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_assets
    rtype_client_constants
    rtype_client_module_constants
    rtype_client_platform
)

rtype_enable_warnings(rtype_client_settings_module)
