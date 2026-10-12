add_library(rtype_client_planet_image STATIC PlanetImage.cpp)

target_include_directories(rtype_client_planet_image PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_planet_image
  PUBLIC
    rtype_client_background
  PRIVATE
    rtype_client_pixel_rounding
)

rtype_enable_warnings(rtype_client_planet_image)
