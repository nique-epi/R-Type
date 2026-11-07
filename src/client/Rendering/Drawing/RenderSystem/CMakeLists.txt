add_library(rtype_client_render_system STATIC RenderSystem.cpp)

target_include_directories(rtype_client_render_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_render_system
  PUBLIC
    rtype_engine
    rtype_client_drawing
  PRIVATE
    rtype_client_easing
    rtype_client_interface
    rtype_client_ship_looks
    rtype_client_sprite_look
    rtype_client_sprites
)

rtype_enable_warnings(rtype_client_render_system)
