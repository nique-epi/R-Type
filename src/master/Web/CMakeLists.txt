add_library(rtype_master_web_module STATIC MasterWebModule.cpp)

target_include_directories(rtype_master_web_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_web_module
  PUBLIC
    rtype_engine_commands
    rtype_engine_modules
  PRIVATE
    rtype_master_constants
    rtype_master_http_module
)

rtype_enable_warnings(rtype_master_web_module)
