add_library(rtype_client_effect_spawner STATIC EffectSpawner.cpp)

target_include_directories(rtype_client_effect_spawner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_effect_spawner
  PUBLIC
    rtype_client_camera_shake
    rtype_client_effect_interfaces
    rtype_client_flash_gate
    rtype_client_gain_figures
    rtype_client_hit_effects
    rtype_client_kill_effects
    rtype_client_pilot_effects
    rtype_client_pixel_effects
    rtype_client_shot_effects
    rtype_engine
)

rtype_enable_warnings(rtype_client_effect_spawner)
