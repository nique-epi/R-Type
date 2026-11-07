add_library(rtype_engine_resources STATIC
  ContentFolders.cpp
  EnsureLines.cpp
  ResourceManifestReading.cpp
  ResourceName.cpp
)

target_include_directories(rtype_engine_resources PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_engine_resources
  PUBLIC
    rtype_engine_exceptions
    rtype_engine_command_configuration
  PRIVATE
    rtype_engine_command_interpreter
    rtype_engine_data_files
    rtype_engine_paths
)

rtype_enable_warnings(rtype_engine_resources)
