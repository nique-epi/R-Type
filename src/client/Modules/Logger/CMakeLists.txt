add_library(rtype_client_logger_module STATIC ClientLoggerModule.cpp)

target_include_directories(rtype_client_logger_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_logger_module
  PUBLIC
    rtype_engine_modules
    rtype_logging
    rtype_network_transport
  PRIVATE
    rtype_client_constants
    rtype_client_module_constants
)

rtype_enable_warnings(rtype_client_logger_module)
