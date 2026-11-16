add_library(rtype_master_leaderboard_services STATIC LeaderboardService.cpp)

target_include_directories(rtype_master_leaderboard_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_leaderboard_services
  PUBLIC
    rtype_logging
    rtype_master_leaderboard_models
    rtype_master_leaderboard_repositories
    rtype_master_wallet_contract
  PRIVATE
    rtype_master_constants
    rtype_network_master_economy
)

rtype_enable_warnings(rtype_master_leaderboard_services)
