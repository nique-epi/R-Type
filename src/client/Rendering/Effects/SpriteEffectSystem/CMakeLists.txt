add_library(rtype_client_sprite_effect_system STATIC SpriteEffectSystem.cpp)

target_include_directories(rtype_client_sprite_effect_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sprite_effect_system
  PUBLIC
    rtype_client_effect_interfaces
    rtype_engine
  PRIVATE
    rtype_client_easing
)

rtype_enable_warnings(rtype_client_sprite_effect_system)
