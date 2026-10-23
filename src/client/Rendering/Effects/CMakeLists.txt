add_library(rtype_client_effect_interfaces INTERFACE)

target_include_directories(rtype_client_effect_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_effect_interfaces
  INTERFACE
    rtype_client_camera_interfaces
    rtype_client_ground_effect_interfaces
    rtype_client_rendering_interfaces
    rtype_client_sprite_look
)

add_subdirectory(Ground)
add_subdirectory(SpriteEffects)
add_subdirectory(Camera)
add_subdirectory(MissionStart)
add_subdirectory(FlashBudget)
add_subdirectory(KillChain)
add_subdirectory(GainAttribution)
add_subdirectory(FlashGate)
add_subdirectory(SheetEffects)
add_subdirectory(EffectTargets)
add_subdirectory(GainFigures)
add_subdirectory(HitEffects)
add_subdirectory(ShotEffects)
add_subdirectory(KillEffects)
add_subdirectory(PilotEffects)
add_subdirectory(EffectSpawner)
