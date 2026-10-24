add_library(rtype_client_animation_system STATIC AnimationSystem.cpp)

target_include_directories(rtype_client_animation_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_animation_system
  PUBLIC
    rtype_engine
    rtype_client_sprites
)

rtype_enable_warnings(rtype_client_animation_system)
