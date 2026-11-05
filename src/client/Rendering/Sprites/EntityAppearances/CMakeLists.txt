add_library(rtype_client_entity_appearances STATIC EntityAppearances.cpp)

target_include_directories(rtype_client_entity_appearances PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_entity_appearances
  PUBLIC
    rtype_client_sprites
    rtype_engine_time
    rtype_game_components
    rtype_game_entity_types
)

rtype_enable_warnings(rtype_client_entity_appearances)
