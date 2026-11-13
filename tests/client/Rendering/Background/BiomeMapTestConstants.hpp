#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include "BiomeLayer.hpp"
#include "MapTileVerticesTestConstants.hpp"

/** @brief The rank of the near plane among the layers of the Pit. */
constexpr std::size_t NEAR_PLANE = 2;

/** @brief The project of the Pit holds six maps of three screens of 480
 * pixels: 1440 pixels each. */
constexpr std::array WIDTHS_OF_THE_MAPS_OF_THE_PIT{1440, 1440, 1440,
                                                   1440, 1440, 1440};

/** @brief The near layer of its first map, Fosse_57, holds 630 tiles, read in
 * the file; each is two triangles, six vertices. */
constexpr std::size_t FIRST_MAP_OF_THE_PIT = 0;
constexpr std::size_t TILES_OF_THE_FIRST_NEAR_LAYER = 630;
constexpr std::size_t VERTICES_OF_THE_FIRST_NEAR_LAYER =
    TILES_OF_THE_FIRST_NEAR_LAYER * VERTICES_OF_ONE_TILE;

/** @brief Its first tile lies at (0, 0) in the map, its picture at (0, 0) in
 * the tileset, read in the file. */
constexpr std::size_t FIRST_TILE_CORNER = 0;
constexpr sf::Vector2f CORNER_OF_THE_MAP{0.0F, 0.0F};

/** @brief A biome whose only layer names a layer no map of the Pit has. */
constexpr std::array LAYERS_THE_PIT_LACKS{
    rtype::client::BiomeLayer{.layer = "Nowhere", .tileset = "none.png"}};
