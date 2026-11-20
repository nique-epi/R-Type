add_library(rtype_client_casino_roulette_table STATIC
  ClothPainting.cpp
  DrawnNumbersPainting.cpp
  PocketColors.cpp
  RouletteBandPainting.cpp
  RouletteBars.cpp
  RoulettePainting.cpp
  RouletteScene.cpp
  RouletteScreen.cpp
  RouletteTexts.cpp
  TableRosterPainting.cpp
)

target_include_directories(rtype_client_casino_roulette_table PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_casino_roulette_table
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_account_session
    rtype_client_application
    rtype_client_audio_contract_service
    rtype_client_audio_contract_sounds
    rtype_client_casino_painting
    rtype_client_casino_roulette_bets
    rtype_client_casino_roulette_cloth
    rtype_client_casino_roulette_wheel
    rtype_client_casino_channel
    rtype_client_interface
    rtype_client_key_chip
    rtype_client_screens_module
    rtype_client_settings_values
    rtype_client_window_display
    rtype_engine_time
  PRIVATE
    rtype_client_easing
    rtype_client_box_corners
    rtype_client_economy
    rtype_client_game_module
    rtype_client_interface_pixel
    rtype_client_widgets
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_casino_roulette_table)
