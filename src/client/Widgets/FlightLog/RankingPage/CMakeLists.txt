add_library(rtype_client_ranking_page STATIC RankingPage.cpp)

target_include_directories(rtype_client_ranking_page PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_ranking_page
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_easing
    rtype_client_entity_sprites
    rtype_client_flight_log_timeline
    rtype_client_outline
    rtype_client_padded_score
    rtype_client_pilot_color
    rtype_client_pilot_name
    rtype_client_player_shading
    rtype_client_typing
)

rtype_enable_warnings(rtype_client_ranking_page)
