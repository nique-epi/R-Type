add_library(rtype_game_enemy_missile_start_position STATIC EnemyMissileStartPosition.cpp)

target_include_directories(rtype_game_enemy_missile_start_position PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_enemy_missile_start_position
  PUBLIC
    rtype_game_components
  PRIVATE
    rtype_engine_collision
)

rtype_enable_warnings(rtype_game_enemy_missile_start_position)
