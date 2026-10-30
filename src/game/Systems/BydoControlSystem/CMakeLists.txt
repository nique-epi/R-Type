add_library(rtype_game_bydo_control_system STATIC BydoControlSystem.cpp)

target_include_directories(rtype_game_bydo_control_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_game_bydo_control_system
  PUBLIC
    rtype_engine_systems
    rtype_engine_component_registry
    rtype_game_components
)

rtype_enable_warnings(rtype_game_bydo_control_system)
