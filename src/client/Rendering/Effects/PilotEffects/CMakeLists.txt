add_library(rtype_client_pilot_effects STATIC PilotEffects.cpp)

target_include_directories(rtype_client_pilot_effects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_pilot_effects
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine
  PRIVATE
    rtype_client_effect_targets
    rtype_client_flash_gate
    rtype_client_mission_start_interfaces
    rtype_client_pixel_effects
)

rtype_enable_warnings(rtype_client_pilot_effects)
