add_library(rtype_client_squadron STATIC Squadron.cpp)

target_include_directories(rtype_client_squadron PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_squadron
  PUBLIC
    rtype_client_interface
    rtype_engine_time
  PRIVATE
    rtype_client_box_corners
    rtype_client_easing
    rtype_client_entity_sprites
    rtype_client_player_shading
    rtype_client_random_range
)

rtype_enable_warnings(rtype_client_squadron)
