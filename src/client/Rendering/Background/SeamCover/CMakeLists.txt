add_library(rtype_client_seam_cover STATIC SeamCover.cpp)

target_include_directories(rtype_client_seam_cover PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_seam_cover
  PUBLIC
    SFML::Graphics
    rtype_client_pixel_batch
  PRIVATE
    rtype_client_background
    rtype_client_dither
)

rtype_enable_warnings(rtype_client_seam_cover)
