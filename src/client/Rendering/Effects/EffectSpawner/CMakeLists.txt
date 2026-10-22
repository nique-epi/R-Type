add_library(rtype_client_effect_spawner STATIC EffectSpawner.cpp)

target_include_directories(rtype_client_effect_spawner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_effect_spawner
  PUBLIC
    rtype_client_camera_shake
    rtype_client_effect_interfaces
    rtype_client_flash_budget
    rtype_client_gain_attribution
    rtype_client_kill_chain
    rtype_client_pixel_effects
    rtype_engine
)

rtype_enable_warnings(rtype_client_effect_spawner)
