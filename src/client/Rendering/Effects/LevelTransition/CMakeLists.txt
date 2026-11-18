add_library(rtype_client_level_transition STATIC LevelTransition.cpp LevelTransitionScene.cpp)

target_include_directories(rtype_client_level_transition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_level_transition
  PUBLIC
    rtype_engine_time
  PRIVATE
    rtype_client_easing
    rtype_client_mission_start_interfaces
    rtype_game_level
)

rtype_enable_warnings(rtype_client_level_transition)
