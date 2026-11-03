add_subdirectory(Players)
add_subdirectory(Match)

add_library(rtype_server_rules_module STATIC RtypeRulesModule.cpp)

target_include_directories(rtype_server_rules_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_rules_module
  PUBLIC
    rtype_engine_install_context
    rtype_server_instances
    rtype_server_rules_match
    rtype_server_rules_players
)

rtype_enable_warnings(rtype_server_rules_module)
