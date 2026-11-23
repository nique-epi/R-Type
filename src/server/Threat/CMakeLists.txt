add_subdirectory(Contract)

if(NOT RTYPE_MODULE_THREAT)
  return()
endif()

add_library(rtype_server_threat_module STATIC ThreatModule.cpp)

target_include_directories(rtype_server_threat_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_threat_module
  PUBLIC
    rtype_engine_install_context
    rtype_game_threat
    rtype_server_instances
    rtype_server_threat_contract
)

rtype_enable_warnings(rtype_server_threat_module)
