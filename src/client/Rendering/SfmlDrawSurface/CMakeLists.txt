add_library(rtype_client_sfml_draw_surface STATIC SfmlDrawSurface.cpp)

target_include_directories(rtype_client_sfml_draw_surface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sfml_draw_surface
  PUBLIC
    rtype_client_rendering_interfaces
    rtype_logging
    SFML::Graphics
)

rtype_enable_warnings(rtype_client_sfml_draw_surface)
