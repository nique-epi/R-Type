add_library(rtype_master_logger_module STATIC MasterLoggerModule.cpp)

target_include_directories(rtype_master_logger_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_logger_module
  PUBLIC
    rtype_engine_modules
    rtype_logging
  PRIVATE
    rtype_master_constants
)

rtype_enable_warnings(rtype_master_logger_module)
