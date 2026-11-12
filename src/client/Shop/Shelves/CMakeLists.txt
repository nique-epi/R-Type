add_library(rtype_client_shop_shelves STATIC
  ShopCatalogue.cpp
  ShopLayout.cpp
  ShopSelection.cpp
)

target_include_directories(rtype_client_shop_shelves PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_shop_shelves
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_row_window
  PRIVATE
    rtype_client_economy
    rtype_identifiers
)

rtype_enable_warnings(rtype_client_shop_shelves)
