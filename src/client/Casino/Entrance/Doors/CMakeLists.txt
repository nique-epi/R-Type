add_library(rtype_client_casino_entrance_doors STATIC
  ButlerMotion.cpp
  CasinoEntrancePainting.cpp
  CasinoEntranceScreen.cpp
)

target_include_directories(rtype_client_casino_entrance_doors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_entrance_doors
  PUBLIC
    rtype_client_casino_entrance_lights
    SFML::Graphics
    SFML::Window
    rtype_client_application
    rtype_client_audio_contract_service
    rtype_client_audio_contract_sounds
    rtype_client_casino_painting
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_settings_values
    rtype_client_spoken_line
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_client_entity_sprites
)

rtype_enable_warnings(rtype_client_casino_entrance_doors)
