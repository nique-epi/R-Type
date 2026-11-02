add_library(rtype_client_leaderboard_screen STATIC LeaderboardScreen.cpp)

target_include_directories(rtype_client_leaderboard_screen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_leaderboard_screen
  PUBLIC
    rtype_client_connection
    rtype_client_input
    rtype_client_interface
    rtype_client_item_list
    rtype_client_key_chip
    rtype_client_leaderboard_filter_dialog
    rtype_client_screen_interfaces
    rtype_client_tab_strip
    rtype_client_window
    SFML::Graphics
  PRIVATE
    rtype_client_animation
    rtype_client_cut_corner_box
    rtype_client_effect_interfaces
    rtype_client_entity_sprites
    rtype_client_interface_pixel
    rtype_client_leaderboard_presentation
    rtype_client_menu_builder
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_leaderboard_screen)
