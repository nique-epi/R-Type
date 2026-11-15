add_library(rtype_client_sky_picture STATIC SkyPicture.cpp)

target_include_directories(rtype_client_sky_picture PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sky_picture
  PUBLIC
    SFML::Graphics
    rtype_client_background
    rtype_client_enlarged_picture
  PRIVATE
    rtype_client_exceptions
    rtype_client_pixel_rounding
    rtype_client_shifted_view
)

rtype_enable_warnings(rtype_client_sky_picture)
