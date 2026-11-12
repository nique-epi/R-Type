add_library(rtype_client_shop_painting STATIC
  PriceTag.cpp
  ShopPainting.cpp
)

target_include_directories(rtype_client_shop_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_shop_painting
  PUBLIC
    rtype_client_interface
    rtype_client_row_window
    rtype_client_settings_values
    rtype_client_shop_shelves
  PRIVATE
    rtype_client_background
    rtype_client_economy
    rtype_client_entity_sprites
    rtype_client_kopecks_painting
    rtype_client_outline
    rtype_client_pixel_shapes
    rtype_client_scroll_mark
    rtype_client_ship_hulls
    rtype_client_ship_looks
    rtype_client_widgets
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_shop_painting)
