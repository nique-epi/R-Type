add_library(rtype_client_leaderboard_filter_dialog STATIC LeaderboardFilterDialog.cpp)

target_include_directories(rtype_client_leaderboard_filter_dialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_leaderboard_filter_dialog
  PUBLIC
    rtype_client_audio_interfaces
    rtype_client_interface
    rtype_client_menu
    rtype_client_screen_interfaces
    rtype_client_window
    rtype_engine_time
    SFML::Window
  PRIVATE
    rtype_client_animation
    rtype_client_choice_selector
    rtype_client_effect_interfaces
    rtype_client_menu_builder
    rtype_client_widgets
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_leaderboard_filter_dialog)
