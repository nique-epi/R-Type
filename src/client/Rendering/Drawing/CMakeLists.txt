add_library(rtype_client_drawing INTERFACE)

target_include_directories(rtype_client_drawing INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_drawing
  INTERFACE
    rtype_client_interface
    rtype_client_sprite_look
)

add_subdirectory(RenderSystem)
add_subdirectory(SfmlDrawSurface)
add_subdirectory(EntityLayer)
