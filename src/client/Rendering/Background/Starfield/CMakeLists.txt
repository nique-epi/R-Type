add_library(rtype_client_starfield STATIC Starfield.cpp)

target_include_directories(rtype_client_starfield PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_starfield
  PUBLIC
    rtype_client_background
    rtype_client_planet_image
    rtype_client_sky_event_scheduler
  PRIVATE
    rtype_client_pixel_rounding
    rtype_client_random_range
)

rtype_enable_warnings(rtype_client_starfield)
