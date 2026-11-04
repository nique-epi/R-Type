add_library(rtype_client_enemy_appearance STATIC EnemyAppearance.cpp)

target_include_directories(rtype_client_enemy_appearance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_enemy_appearance
  PUBLIC
    rtype_client_sprites
    rtype_engine
    rtype_game_components
    rtype_game_enemy_kind
)

rtype_enable_warnings(rtype_client_enemy_appearance)
