add_library(rtype_client_cinema_projection STATIC CinemaProjection.cpp)

target_include_directories(rtype_client_cinema_projection PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_cinema_projection
  PUBLIC
    rtype_client_intro_interfaces
  PRIVATE
    rtype_client_pixel_rounding
)

rtype_enable_warnings(rtype_client_cinema_projection)
