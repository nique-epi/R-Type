add_library(rtype_master_leaderboard_controllers STATIC LeaderboardController.cpp)

target_include_directories(rtype_master_leaderboard_controllers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_leaderboard_controllers
  PUBLIC
    rtype_master_http_module
    rtype_master_leaderboard_services
    rtype_master_servers_controllers
  PRIVATE
    rtype_network_master_routes
    rtype_network_master_validators
    rtype_network_wire_types
)

rtype_enable_warnings(rtype_master_leaderboard_controllers)
