find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(rtype_master_http_module STATIC
  HealthResponse.cpp
  MasterHttpServer.cpp
  ProblemDetails.cpp
)

target_include_directories(rtype_master_http_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_http_module
  PUBLIC
    rtype_logging
  PRIVATE
    httplib::httplib
    nlohmann_json::nlohmann_json
    rtype_master_constants
    rtype_master_exceptions
    rtype_network_constants
)

rtype_enable_warnings(rtype_master_http_module)
