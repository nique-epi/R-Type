add_library(rtype_engine_commands STATIC
  CommandInterpreter.cpp
  CommandLine.cpp
  ConfigurationLog.cpp
  ConfigurationReader.cpp
  ConfigurationRegistry.cpp
  SettingLine.cpp
  SettingValue.cpp
)

target_include_directories(rtype_engine_commands PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_commands PUBLIC rtype_engine_exceptions rtype_logging)

rtype_enable_warnings(rtype_engine_commands)
