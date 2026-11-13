add_library(rtype_client_ship_hulls STATIC ShipHulls.cpp)

target_include_directories(rtype_client_ship_hulls PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ship_hulls
  PUBLIC
    rtype_client_sprites
    rtype_identifiers
  PRIVATE
    rtype_client_entity_sprites
    rtype_client_pilot_color
    rtype_engine
    rtype_game
)

rtype_enable_warnings(rtype_client_ship_hulls)
