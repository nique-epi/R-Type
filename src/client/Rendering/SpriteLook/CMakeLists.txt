add_library(rtype_client_sprite_look INTERFACE)

target_include_directories(rtype_client_sprite_look INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sprite_look
  INTERFACE
    rtype_client_interface
    rtype_game_components
    rtype_engine_time
)

add_subdirectory(HitFlashSystem)
add_subdirectory(ShipFlightSystem)
