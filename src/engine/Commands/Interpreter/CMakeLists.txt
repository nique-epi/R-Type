add_library(rtype_engine_command_interpreter STATIC
  CommandInterpreter.cpp
  CommandLine.cpp
  NameList.cpp
)

target_include_directories(rtype_engine_command_interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_command_interpreter PUBLIC rtype_engine_exceptions)

rtype_enable_warnings(rtype_engine_command_interpreter)
