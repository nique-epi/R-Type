add_library(rtype_master_storage_module STATIC MasterStorageModule.cpp)

target_include_directories(rtype_master_storage_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_storage_module
  PUBLIC
    rtype_engine_commands
    rtype_engine_modules
    rtype_logging
    rtype_storage
  PRIVATE
    rtype_engine_paths
    rtype_master_constants
)

rtype_enable_warnings(rtype_master_storage_module)
