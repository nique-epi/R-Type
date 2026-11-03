add_subdirectory(Values)
add_subdirectory(Definitions)
add_subdirectory(Storage)

add_library(rtype_client_settings_module STATIC
  ClientSettingsModule.cpp
  SettingsService.cpp
)

target_include_directories(rtype_client_settings_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_settings_module
  PUBLIC
    SFML::Window
    rtype_client_input_keys
    rtype_client_settings_definitions
    rtype_client_settings_storage
    rtype_client_settings_values
    rtype_engine_install_context
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_client_constants
    rtype_client_platform
    rtype_engine_paths
)

rtype_enable_warnings(rtype_client_settings_module)
