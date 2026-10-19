add_library(rtype_client_sprite_showcase STATIC SpriteShowcase.cpp)

target_include_directories(rtype_client_sprite_showcase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sprite_showcase
  PUBLIC
    rtype_client_effect_spawner
    rtype_client_rendering_interfaces
    rtype_engine
  PRIVATE
    rtype_client_entity_sprites
)

rtype_enable_warnings(rtype_client_sprite_showcase)
