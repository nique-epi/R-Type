add_library(rtype_client_ground_effect_interfaces INTERFACE)

target_include_directories(rtype_client_ground_effect_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ground_effect_interfaces INTERFACE rtype_client_rendering_interfaces)

add_library(rtype_client_ground_effects STATIC GroundEffects.cpp)

target_link_libraries(rtype_client_ground_effects
  PUBLIC
    rtype_client_ground_effect_interfaces
    rtype_engine
  PRIVATE
    rtype_client_easing
    rtype_client_pixel_rounding
)

rtype_enable_warnings(rtype_client_ground_effects)

add_subdirectory(PixelEffects)
add_subdirectory(GroundEffectSystem)
