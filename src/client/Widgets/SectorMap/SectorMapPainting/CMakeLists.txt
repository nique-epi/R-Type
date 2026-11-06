add_library(rtype_client_sector_map_painting STATIC SectorMapPainting.cpp)

target_include_directories(rtype_client_sector_map_painting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_sector_map_painting
  PUBLIC
    rtype_client_sector_map_interfaces
  PRIVATE
    rtype_client_box_corners
    rtype_client_cut_corner_box
    rtype_client_outline
    rtype_client_ringed_planet
    rtype_client_sector_layout
)

rtype_enable_warnings(rtype_client_sector_map_painting)
