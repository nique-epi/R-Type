add_library(rtype_client_hangar_locker STATIC
  LockerChoice.cpp
  LockerPainting.cpp
  MetalTexture.cpp
)

target_include_directories(rtype_client_hangar_locker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_hangar_locker
  PUBLIC
    SFML::Window
    rtype_client_background
    rtype_client_interface
    rtype_client_row_window
    rtype_client_settings_values
  PRIVATE
    rtype_client_dither
    rtype_client_entity_sprites
    rtype_client_outline
    rtype_client_scroll_mark
    rtype_client_ship_hulls
    rtype_client_pixel_shapes
    rtype_client_random_range
    rtype_client_ship_looks
)

rtype_enable_warnings(rtype_client_hangar_locker)
