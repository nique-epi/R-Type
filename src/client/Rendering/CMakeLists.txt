add_library(rtype_client_rendering_interfaces INTERFACE)

target_include_directories(rtype_client_rendering_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_rendering_interfaces INTERFACE rtype_game_components)

add_subdirectory(RenderSystem)
add_subdirectory(SfmlDrawSurface)
add_subdirectory(Background)

add_library(rtype_client_rendering INTERFACE)

target_link_libraries(rtype_client_rendering
  INTERFACE
    rtype_client_rendering_interfaces
    rtype_client_render_system
    rtype_client_sfml_draw_surface
)
