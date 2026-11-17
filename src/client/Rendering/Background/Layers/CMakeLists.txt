find_package(LDtkLoader CONFIG REQUIRED)

add_library(rtype_client_background_layers STATIC
  EmissionGroup.cpp
  ImageLayer.cpp
  LayerBuilding.cpp
  LayeredBackground.cpp
  LdtkProjectFile.cpp
  MapLayer.cpp
  ParticleLayer.cpp
  ParticleMotion.cpp
  ParticlePainting.cpp
)

target_include_directories(rtype_client_background_layers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(rtype_client_background_layers
  PUBLIC
    SFML::Graphics
    rtype_client_background
    rtype_client_background_description
    rtype_client_map_plane
    rtype_engine_time
  PRIVATE
    LDtkLoader::LDtkLoader
    rtype_client_map_columns
    rtype_client_map_tile_vertices
    rtype_client_pixel_rounding
    rtype_client_random_range
    rtype_logging
)

rtype_enable_warnings(rtype_client_background_layers)
