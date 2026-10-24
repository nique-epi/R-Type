add_library(rtype_client_entity_sprites STATIC EntitySprites.cpp)

target_include_directories(rtype_client_entity_sprites PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_entity_sprites
  PUBLIC
    rtype_client_sprites
  PRIVATE
    rtype_engine
    rtype_game
)

rtype_enable_warnings(rtype_client_entity_sprites)
