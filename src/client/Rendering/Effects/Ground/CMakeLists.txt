add_library(rtype_client_ground_effect_interfaces INTERFACE)

target_include_directories(rtype_client_ground_effect_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ground_effect_interfaces
  INTERFACE
    rtype_client_rendering_interfaces
    rtype_client_sprite_look
    rtype_engine_collision
    rtype_game_ship_control_constants
)

add_subdirectory(GroundEffects)
add_subdirectory(PixelEffects)
add_subdirectory(GroundEffectSystem)
