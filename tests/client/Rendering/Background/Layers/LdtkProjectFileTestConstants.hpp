#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <string_view>
#include "MapTileVerticesTestConstants.hpp"

/** @brief The project of the Pit the repository ships, below the assets
 * folder, and its nearest tile layer. It holds six maps of three screens of
 * 480 pixels: 1440 pixels each. */
constexpr std::string_view PIT_PROJECT = "biomes/fosse/fosse.ldtk";
constexpr std::string_view NEAR_TILE_LAYER = "Near";
constexpr std::array WIDTHS_OF_THE_MAPS_OF_THE_PIT{1440, 1440, 1440,
                                                   1440, 1440, 1440};

/** @brief The near layer of its first map, Fosse_57, holds 661 tiles, read in
 * the file; each is two triangles, six vertices. Its first tile lies at
 * (112, 0) in the map, where the entrance of the cave begins, its picture at
 * (0, 0) in the tileset. */
constexpr std::size_t FIRST_MAP_OF_THE_PIT = 0;
constexpr std::size_t TILES_OF_THE_FIRST_NEAR_LAYER = 661;
constexpr std::size_t VERTICES_OF_THE_FIRST_NEAR_LAYER =
    TILES_OF_THE_FIRST_NEAR_LAYER * VERTICES_OF_ONE_TILE;
constexpr std::size_t FIRST_TILE_CORNER = 0;
constexpr sf::Vector2f WHERE_THE_ENTRANCE_BEGINS{112.0F, 0.0F};
constexpr sf::Vector2f CORNER_OF_THE_TILESET{0.0F, 0.0F};
constexpr int COLUMN_WHERE_THE_ENTRANCE_BEGINS = 112;

/** @brief A project no folder holds, and a layer no map of the Pit has. */
constexpr std::string_view MISSING_PROJECT = "biomes/nowhere/nowhere.ldtk";
constexpr std::string_view LAYER_THE_PIT_LACKS = "Nowhere";

/**
 * @brief The project of the tests, props.ldtk, next to them: one map of 1440
 * pixels whose entity layer Mobs holds a crate, an entity of 16 by 16 pixels
 * with a tile at (16, 0) of its tileset, its pivot at the middle of its
 * bottom side, placed at (104, 64), and a marker, an entity without a tile.
 * The crate so covers (96, 48) to (112, 64).
 */
constexpr std::string_view PROP_PROJECT_FILE = "props.ldtk";
constexpr std::string_view PROP_LAYER = "Mobs";
constexpr std::array WIDTH_OF_THE_PROP_MAP{1440};
constexpr std::size_t THE_PROP_MAP = 0;
constexpr std::size_t VERTICES_OF_ONE_PROP = 6;
constexpr sf::Vector2f TOP_LEFT_OF_THE_CRATE{96.0F, 48.0F};
constexpr sf::Vector2f BOTTOM_RIGHT_OF_THE_CRATE{112.0F, 64.0F};
constexpr sf::Vector2f TOP_LEFT_OF_ITS_TILE{16.0F, 0.0F};
constexpr sf::Vector2f BOTTOM_RIGHT_OF_ITS_TILE{32.0F, 16.0F};
