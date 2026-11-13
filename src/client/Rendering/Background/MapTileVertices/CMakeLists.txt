add_library(rtype_client_map_tile_vertices STATIC MapTileVertices.cpp)

target_include_directories(rtype_client_map_tile_vertices PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_map_tile_vertices
  PUBLIC
    rtype_client_background
)

rtype_enable_warnings(rtype_client_map_tile_vertices)
