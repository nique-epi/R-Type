add_library(rtype_client_label_placement STATIC LabelPlacement.cpp)

target_include_directories(rtype_client_label_placement PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_label_placement
  PUBLIC
    rtype_client_sector_map_interfaces
)

rtype_enable_warnings(rtype_client_label_placement)
