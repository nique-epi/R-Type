add_subdirectory(Paths)
add_subdirectory(Attacks)

add_library(rtype_game_behaviours_movements STATIC BuiltInBehaviours.cpp)

target_include_directories(rtype_game_behaviours_movements PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_behaviours_movements
  PUBLIC
    rtype_game_behaviours_piloting
    rtype_game_behaviours_paths
    rtype_game_behaviours_attacks
)

rtype_enable_warnings(rtype_game_behaviours_movements)
