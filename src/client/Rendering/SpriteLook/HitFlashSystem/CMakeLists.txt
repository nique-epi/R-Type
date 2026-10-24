add_library(rtype_client_hit_flash_system STATIC HitFlashSystem.cpp)

target_include_directories(rtype_client_hit_flash_system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_hit_flash_system
  PUBLIC
    rtype_engine
    rtype_client_sprite_look
)

rtype_enable_warnings(rtype_client_hit_flash_system)
