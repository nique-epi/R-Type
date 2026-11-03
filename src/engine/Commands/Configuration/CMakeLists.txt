add_library(rtype_engine_command_configuration STATIC
  ConfigurationLog.cpp
  ConfigurationReader.cpp
)

target_include_directories(rtype_engine_command_configuration PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_command_configuration
  PUBLIC
    rtype_engine_command_interpreter
    rtype_logging
)

rtype_enable_warnings(rtype_engine_command_configuration)
