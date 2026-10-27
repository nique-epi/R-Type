add_library(rtype_client_mission_log_page STATIC MissionLogPage.cpp)

target_include_directories(rtype_client_mission_log_page PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_mission_log_page
  PUBLIC
    rtype_client_flight_log_interfaces
  PRIVATE
    rtype_client_easing
    rtype_client_entity_sprites
    rtype_client_final_ranking
    rtype_client_outline
    rtype_client_padded_score
    rtype_client_typing
    rtype_client_widget_text
    rtype_network_protocol
)

rtype_enable_warnings(rtype_client_mission_log_page)
