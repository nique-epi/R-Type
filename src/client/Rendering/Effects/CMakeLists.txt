add_library(rtype_client_effect_interfaces INTERFACE)

target_include_directories(rtype_client_effect_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_effect_interfaces INTERFACE rtype_client_rendering_interfaces)

add_library(rtype_client_easing STATIC Easing.cpp)

target_link_libraries(rtype_client_easing
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine_time
)

rtype_enable_warnings(rtype_client_easing)

add_library(rtype_client_ground_effects STATIC GroundEffects.cpp)

target_link_libraries(rtype_client_ground_effects
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine
  PRIVATE
    rtype_client_easing
    rtype_client_pixel_rounding
)

rtype_enable_warnings(rtype_client_ground_effects)

add_library(rtype_client_mission_start STATIC MissionStart.cpp)

target_link_libraries(rtype_client_mission_start
  PUBLIC
    rtype_client_effect_interfaces
  PRIVATE
    rtype_client_easing
)

rtype_enable_warnings(rtype_client_mission_start)

add_subdirectory(FlashBudget)
add_subdirectory(KillChain)
add_subdirectory(GainAttribution)
add_subdirectory(CameraShake)
add_subdirectory(PixelEffects)
add_subdirectory(SpriteEffectSystem)
add_subdirectory(GroundEffectSystem)
add_subdirectory(EffectSpawner)
