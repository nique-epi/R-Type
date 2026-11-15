add_library(rtype_client_casino_entrance STATIC
  ButlerMotion.cpp
  CasinoEntrancePainting.cpp
  CasinoEntranceScreen.cpp
)

target_include_directories(rtype_client_casino_entrance PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_entrance
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_application
    rtype_client_audio_contract_sounds
    rtype_client_casino_painting
    rtype_client_connection_casino
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_settings_values
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_client_entity_sprites
)

rtype_enable_warnings(rtype_client_casino_entrance)
