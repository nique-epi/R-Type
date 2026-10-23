add_library(rtype_client_sprites INTERFACE)

target_include_directories(rtype_client_sprites INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sprites
  INTERFACE
    rtype_client_animation
    rtype_client_rendering_interfaces
)

add_subdirectory(Animation)
add_subdirectory(EntitySprites)
add_subdirectory(AnimationSystem)
add_subdirectory(AssetTextureSource)
add_subdirectory(PilotColor)
