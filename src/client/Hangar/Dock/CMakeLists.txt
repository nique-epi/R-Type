add_library(rtype_client_hangar_dock STATIC
  ArmPainting.cpp
  DockPainting.cpp
  FittingTimeline.cpp
)

target_include_directories(rtype_client_hangar_dock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_hangar_dock
  PUBLIC
    rtype_client_hangar_locker
    rtype_client_interface
    rtype_client_settings_values
    rtype_engine_time
  PRIVATE
    rtype_client_easing
    rtype_client_entity_sprites
    rtype_client_outline
    rtype_client_pixel_rounding
    rtype_client_pixel_shapes
    rtype_client_ship_hulls
    rtype_client_ship_looks
    rtype_client_ship_picture
    rtype_client_widgets
)

rtype_enable_warnings(rtype_client_hangar_dock)
