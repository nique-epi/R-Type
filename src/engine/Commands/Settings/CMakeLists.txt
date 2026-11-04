add_library(rtype_engine_command_settings STATIC
  ConfigurationRegistry.cpp
  SettingLine.cpp
  SettingOptions.cpp
  SettingValue.cpp
)

target_include_directories(rtype_engine_command_settings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_command_settings
  PUBLIC
    rtype_engine_command_interpreter
    rtype_engine_exceptions
    rtype_engine_launch_arguments
)

rtype_enable_warnings(rtype_engine_command_settings)
