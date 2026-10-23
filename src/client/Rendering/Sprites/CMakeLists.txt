add_library(rtype_client_sprites INTERFACE)

target_include_directories(rtype_client_sprites INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sprites
  INTERFACE
    rtype_client_animation
    rtype_client_rendering_interfaces
)

add_library(rtype_client_entity_sprites STATIC EntitySprites.cpp)

target_link_libraries(rtype_client_entity_sprites
  PUBLIC
    rtype_client_sprites
  PRIVATE
    rtype_engine
    rtype_game
)

rtype_enable_warnings(rtype_client_entity_sprites)

add_subdirectory(Animation)
add_subdirectory(AnimationSystem)
add_subdirectory(AssetTextureSource)
