add_library(rtype_game_collision_system STATIC CollisionSystem.cpp)

target_include_directories(rtype_game_collision_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_collision_system
  PUBLIC
    rtype_engine_systems
    rtype_engine_component_registry
    rtype_engine_event_bus
    rtype_engine_collision
    rtype_game_components
)

rtype_enable_warnings(rtype_game_collision_system)
