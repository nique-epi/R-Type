add_library(rtype_game_behaviours_rtype STATIC
  RtypeBehaviours.cpp
  StraightBehaviour.cpp
  WeaveBehaviour.cpp
)

target_include_directories(rtype_game_behaviours_rtype PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_behaviours_rtype
  PUBLIC
    rtype_game_behaviours_piloting
    rtype_game_components
  PRIVATE
    rtype_game_spawning_constants
)

rtype_enable_warnings(rtype_game_behaviours_rtype)
