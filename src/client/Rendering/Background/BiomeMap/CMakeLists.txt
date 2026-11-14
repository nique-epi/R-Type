find_package(LDtkLoader CONFIG REQUIRED)

add_library(rtype_client_biome_map STATIC BiomeMap.cpp)

target_include_directories(rtype_client_biome_map PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_biome_map
  PUBLIC
    rtype_client_background
    rtype_client_map_plane
  PRIVATE
    LDtkLoader::LDtkLoader
    rtype_client_exceptions
    rtype_client_map_tile_vertices
    rtype_logging
)

rtype_enable_warnings(rtype_client_biome_map)
