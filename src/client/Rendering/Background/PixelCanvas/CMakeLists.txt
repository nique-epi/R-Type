add_library(rtype_client_pixel_canvas STATIC PixelCanvas.cpp)

target_include_directories(rtype_client_pixel_canvas PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_pixel_canvas
  PUBLIC
    SFML::Graphics
  PRIVATE
    rtype_client_background
    rtype_client_exceptions
    rtype_client_integer_scale
)

rtype_enable_warnings(rtype_client_pixel_canvas)
