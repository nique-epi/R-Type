add_library(rtype_client_hangar_bay STATIC HangarBay.cpp)

target_include_directories(rtype_client_hangar_bay PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_hangar_bay
  PUBLIC
    rtype_client_player_shading
    rtype_client_widgets
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_entity_sprites
    rtype_client_player_number_label
    rtype_client_sunk_box
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_hangar_bay)
