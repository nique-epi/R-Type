add_library(rtype_game_behaviours_piloting STATIC
  BehaviourRegistry.cpp
  BehaviourSteering.cpp
)

target_include_directories(rtype_game_behaviours_piloting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_behaviours_piloting
  PUBLIC
    rtype_engine_time
    rtype_game_components
  PRIVATE
    rtype_game_exceptions
)

rtype_enable_warnings(rtype_game_behaviours_piloting)
