add_library(rtype_server_logger_module STATIC ServerLoggerModule.cpp)

target_include_directories(rtype_server_logger_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_logger_module
  PUBLIC
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_server_constants
    rtype_server_module_constants
)

rtype_enable_warnings(rtype_server_logger_module)
