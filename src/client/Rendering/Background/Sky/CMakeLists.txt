add_library(rtype_client_sky STATIC Sky.cpp)

target_include_directories(rtype_client_sky PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sky
  PUBLIC
    rtype_client_background
    rtype_client_background_composition
    rtype_client_background_description
    rtype_client_background_layers
    rtype_client_gaussian_blur
    rtype_client_seam_cover
    rtype_client_sky_picture
)

rtype_enable_warnings(rtype_client_sky)
