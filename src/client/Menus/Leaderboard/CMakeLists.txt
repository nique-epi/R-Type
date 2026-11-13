add_library(rtype_client_menus_leaderboard STATIC
  LeaderboardFilterDialog.cpp
  LeaderboardFrames.cpp
  LeaderboardPresentation.cpp
  LeaderboardScreen.cpp
)

target_include_directories(rtype_client_menus_leaderboard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_menus_leaderboard
  PUBLIC
    SFML::Graphics
    SFML::Window
    rtype_client_animation
    rtype_client_application
    rtype_client_audio_contract_sounds
    rtype_client_background
    rtype_client_connection_leaderboard
    rtype_client_ground_effect_interfaces
    rtype_client_handshake
    rtype_client_input_keys
    rtype_client_interface
    rtype_client_item_list
    rtype_client_key_chip
    rtype_client_menu
    rtype_client_tab_strip
    rtype_client_widgets
    rtype_client_window_display
    rtype_engine_time
    rtype_identifiers
    rtype_network_leaderboard_request
    rtype_network_protocol
  PRIVATE
    rtype_client_button
    rtype_client_choice_selector
    rtype_client_common_screens
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_interface_pixel
    rtype_client_menu_builder
    rtype_client_row_control
    rtype_client_screens_module
    rtype_client_text_measures
    rtype_client_widget_text
    rtype_game_components
    rtype_network_leaderboard_answer
)

rtype_enable_warnings(rtype_client_menus_leaderboard)
