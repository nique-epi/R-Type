add_subdirectory(Shelves)
add_subdirectory(Painting)

add_library(rtype_client_shop_module STATIC
  ClientShopModule.cpp
  ShopScreen.cpp
  ShopStatus.cpp
)

target_include_directories(rtype_client_shop_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_shop_module
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_economy
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_settings_module
    rtype_client_shop_shelves
    rtype_engine_modules
  PRIVATE
    rtype_client_constants
    rtype_client_interface_pixel
    rtype_client_menu_builder
    rtype_client_ship_looks
    rtype_client_shop_painting
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_shop_module)
