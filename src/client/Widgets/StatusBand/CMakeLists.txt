add_library(rtype_client_status_band STATIC StatusBand.cpp)

target_include_directories(rtype_client_status_band PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_status_band
  PUBLIC
    rtype_client_replication_interfaces
    rtype_client_widgets
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_outline
    rtype_client_padded_score
    rtype_client_pilot_color
    rtype_client_pilot_name
    rtype_client_player_shading
)

rtype_enable_warnings(rtype_client_status_band)
