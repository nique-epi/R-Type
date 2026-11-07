add_library(rtype_game_behaviours_attacks STATIC
  ActionCycle.cpp
  BydoTyrantBehaviour.cpp
  BydomancerBehaviour.cpp
  DrillbackBehaviour.cpp
  ReaperBehaviour.cpp
)

target_include_directories(rtype_game_behaviours_attacks PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_behaviours_attacks
  PUBLIC
    rtype_game_behaviours_paths
    rtype_game_behaviours_piloting
    rtype_game_components
)

rtype_enable_warnings(rtype_game_behaviours_attacks)
