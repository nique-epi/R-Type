add_library(rtype_game_behaviours_paths STATIC
  ScatterBehaviour.cpp
  SineOffset.cpp
  StraightBehaviour.cpp
  WeaveBehaviour.cpp
)

target_include_directories(rtype_game_behaviours_paths PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_behaviours_paths
  PUBLIC
    rtype_game_behaviours_piloting
    rtype_game_components
    rtype_game_spawning_constants
)

rtype_enable_warnings(rtype_game_behaviours_paths)
