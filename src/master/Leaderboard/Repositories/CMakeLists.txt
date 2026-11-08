add_library(rtype_master_leaderboard_repositories STATIC GameResultRepository.cpp)

target_include_directories(rtype_master_leaderboard_repositories PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_master_leaderboard_repositories
  PUBLIC
    rtype_master_leaderboard_models
    rtype_master_storage_module
  PRIVATE
    rtype_master_servers_repositories
)

rtype_enable_warnings(rtype_master_leaderboard_repositories)
