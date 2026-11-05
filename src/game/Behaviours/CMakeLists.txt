add_library(rtype_game_behaviours STATIC
  BehaviourRegistry.cpp
  BehaviourSteering.cpp
  RtypeBehaviours.cpp
  StraightBehaviour.cpp
  WeaveBehaviour.cpp
)

target_include_directories(rtype_game_behaviours PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_behaviours
  PUBLIC
    rtype_engine_time
    rtype_game_components
  PRIVATE
    rtype_game_exceptions
    rtype_game_spawning_constants
)

rtype_enable_warnings(rtype_game_behaviours)
