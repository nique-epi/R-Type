add_library(rtype_client_background_composition STATIC
  GaussianBlur.cpp
  LayerCanvas.cpp
  SeamCover.cpp
  Sky.cpp
  SkyPicture.cpp
)

target_include_directories(rtype_client_background_composition PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_background_composition
  PUBLIC
    SFML::Graphics
    rtype_client_background
    rtype_client_background_description
    rtype_client_background_layers
    rtype_client_enlarged_picture
    rtype_client_pixel_batch
    rtype_logging
  PRIVATE
    rtype_client_dither
    rtype_client_exceptions
    rtype_client_pixel_rounding
    rtype_client_shifted_view
)

rtype_enable_warnings(rtype_client_background_composition)
