add_library(rtype_client_pixel_effects STATIC PixelEffects.cpp)

target_include_directories(rtype_client_pixel_effects PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_pixel_effects
  PUBLIC
    rtype_client_ground_effect_interfaces
    rtype_engine
  PRIVATE
    rtype_client_random_range
)

rtype_enable_warnings(rtype_client_pixel_effects)
