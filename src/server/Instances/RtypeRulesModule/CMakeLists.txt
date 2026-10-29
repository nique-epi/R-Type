add_library(rtype_server_rtype_rules_module STATIC RtypeRulesModule.cpp)

target_include_directories(rtype_server_rtype_rules_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_server_rtype_rules_module
  PUBLIC
    rtype_engine_install_context
    rtype_server_game_flow
    rtype_server_instances
    rtype_server_score_keeper
  PRIVATE
    rtype_server_rtype_systems
    rtype_server_player_controller
    rtype_server_ship_input
)

rtype_enable_warnings(rtype_server_rtype_rules_module)
