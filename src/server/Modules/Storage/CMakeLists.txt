add_library(rtype_server_storage_module STATIC ServerStorageModule.cpp)

target_include_directories(rtype_server_storage_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_storage_module
  PUBLIC
    rtype_engine_application
    rtype_logging
    rtype_server_storage
  PRIVATE
    rtype_engine_paths
    rtype_server_module_constants
)

rtype_enable_warnings(rtype_server_storage_module)
