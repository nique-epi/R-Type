add_library(rtype_client_entity_layer STATIC EntityLayer.cpp)

target_include_directories(rtype_client_entity_layer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_entity_layer
  PUBLIC
    rtype_client_asset_texture_source
    rtype_client_pixel_batch
    rtype_client_pixel_canvas
    rtype_client_render_system
    rtype_client_drawing
    rtype_client_sfml_draw_surface
    rtype_client_text_writer
  PRIVATE
    rtype_client_asset_library
    rtype_client_ground_effects
    rtype_client_shifted_view
)

rtype_enable_warnings(rtype_client_entity_layer)
