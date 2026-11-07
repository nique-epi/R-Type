find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_master_http_module STATIC
  MasterHttpModule.cpp
  MasterHttpServer.cpp
)

target_include_directories(rtype_master_http_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_http_module
  PUBLIC
    rtype_engine_commands
    rtype_engine_modules
    rtype_logging
    rtype_network_http
    rtype_network_master_routes
    rtype_network_transport
  PRIVATE
    nlohmann_json::nlohmann_json
    rtype_master_constants
    rtype_network_constants
)

rtype_enable_warnings(rtype_master_http_module)
