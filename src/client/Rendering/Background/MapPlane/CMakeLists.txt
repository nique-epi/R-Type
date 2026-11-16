add_library(rtype_client_map_plane STATIC MapPlane.cpp)

target_include_directories(rtype_client_map_plane PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_map_plane
  PUBLIC
    rtype_client_background
  PRIVATE
    rtype_client_map_columns
)

rtype_enable_warnings(rtype_client_map_plane)
