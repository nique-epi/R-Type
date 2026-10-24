add_library(rtype_client_rendering_interfaces INTERFACE)

target_include_directories(rtype_client_rendering_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_rendering_interfaces
  INTERFACE
    rtype_client_background
    rtype_engine_time
    rtype_game_components
    SFML::Graphics
)

add_subdirectory(Easing)
add_subdirectory(PixelCanvas)
add_subdirectory(Shaders)
add_subdirectory(Sprites)
add_subdirectory(Interface)
add_subdirectory(SpriteLook)
add_subdirectory(Drawing)
add_subdirectory(Effects)
add_subdirectory(Background)

add_library(rtype_client_rendering INTERFACE)

target_link_libraries(rtype_client_rendering
  INTERFACE
    rtype_client_rendering_interfaces
    rtype_client_render_system
    rtype_client_sfml_draw_surface
    rtype_client_asset_texture_source
    rtype_client_entity_layer
    rtype_client_entity_sprites
)
