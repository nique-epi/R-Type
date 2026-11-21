add_library(rtype_client_hurt_beam_painter STATIC HurtBeamPainter.cpp)

target_include_directories(rtype_client_hurt_beam_painter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_hurt_beam_painter
  PUBLIC
    rtype_client_pixels
    rtype_engine
    rtype_game_hurt_zone_system
  PRIVATE
    rtype_client_entities
    rtype_client_interface_pixel
    rtype_game_components
)

rtype_enable_warnings(rtype_client_hurt_beam_painter)
