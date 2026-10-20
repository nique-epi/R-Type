add_library(rtype_client_rendering_interfaces INTERFACE)

target_include_directories(rtype_client_rendering_interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_rendering_interfaces
  INTERFACE
    rtype_client_animation
    rtype_client_background
    rtype_game_components
    SFML::Graphics
)

add_library(rtype_client_entity_sprites STATIC EntitySprites.cpp)

target_link_libraries(rtype_client_entity_sprites
  PUBLIC
    rtype_client_rendering_interfaces
  PRIVATE
    rtype_engine
    rtype_game
)

rtype_enable_warnings(rtype_client_entity_sprites)

add_library(rtype_client_fragment_shader STATIC FragmentShader.cpp)

target_link_libraries(rtype_client_fragment_shader
  PUBLIC
    rtype_client_rendering_interfaces
  PRIVATE
    rtype_logging
)

rtype_enable_warnings(rtype_client_fragment_shader)

add_subdirectory(Animation)
add_subdirectory(AnimationSystem)
add_subdirectory(HitFlashSystem)
add_subdirectory(RenderSystem)
add_subdirectory(SfmlDrawSurface)
add_subdirectory(AssetTextureSource)
add_subdirectory(PixelCanvas)
add_subdirectory(InterfaceCanvas)
add_subdirectory(Squadron)
add_subdirectory(EntityLayer)
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
