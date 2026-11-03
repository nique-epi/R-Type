add_library(rtype_client_leaderboard_presentation STATIC LeaderboardPresentation.cpp)

target_include_directories(rtype_client_leaderboard_presentation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_leaderboard_presentation
  PUBLIC
    rtype_client_connection
    rtype_client_effect_interfaces
    rtype_client_interface
    rtype_client_screen_interfaces
    rtype_client_widgets
    rtype_network_protocol
  PRIVATE
    rtype_client_animation
)

rtype_enable_warnings(rtype_client_leaderboard_presentation)
