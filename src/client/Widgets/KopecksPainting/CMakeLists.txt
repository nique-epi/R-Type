add_library(rtype_client_kopecks_painting STATIC KopecksPainting.cpp)

target_include_directories(rtype_client_kopecks_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_kopecks_painting
  PUBLIC
    rtype_client_widgets
  PRIVATE
    rtype_client_cut_corner_box
    rtype_client_economy
    rtype_client_entity_sprites
    rtype_client_widget_text
)

rtype_enable_warnings(rtype_client_kopecks_painting)
