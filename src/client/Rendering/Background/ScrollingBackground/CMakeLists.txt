add_library(rtype_client_scrolling_background STATIC ScrollingBackground.cpp)

target_include_directories(rtype_client_scrolling_background PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_scrolling_background
  PUBLIC
    rtype_client_gaussian_blur
    rtype_client_pixel_batch
    rtype_client_starfield
  PRIVATE
    rtype_client_exceptions
    rtype_client_integer_scale
)

rtype_enable_warnings(rtype_client_scrolling_background)
