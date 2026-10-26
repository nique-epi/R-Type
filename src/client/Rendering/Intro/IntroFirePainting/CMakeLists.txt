add_library(rtype_client_intro_fire_painting STATIC IntroFirePainting.cpp)

target_include_directories(rtype_client_intro_fire_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_intro_fire_painting
  PUBLIC
    rtype_client_intro_interfaces
  PRIVATE
    rtype_client_cinema_projection
    rtype_client_easing
    rtype_client_entity_sprites
    rtype_client_intro_timeline
)

rtype_enable_warnings(rtype_client_intro_fire_painting)
