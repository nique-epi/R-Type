add_library(rtype_game_threat STATIC Firepower.cpp ThreatRank.cpp)

target_include_directories(rtype_game_threat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_threat
  PUBLIC
    rtype_engine_component_registry
    rtype_engine_time
    rtype_game_components
    rtype_game_entity_sheets
  PRIVATE
    rtype_engine_collision_shapes
)

rtype_enable_warnings(rtype_game_threat)
