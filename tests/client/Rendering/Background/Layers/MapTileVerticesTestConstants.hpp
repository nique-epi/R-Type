#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include "MapTile.hpp"

/** @brief A tile of 16 pixels at (32, 16) in its map, whose picture lies at
 * (48, 0) in its tileset. */
constexpr int TILE_SIZE = 16;
constexpr sf::Vector2i TILE_POSITION{32, 16};
constexpr sf::Vector2i TILE_SOURCE{48, 0};
constexpr std::array ONE_TILE{
    rtype::client::MapTile{.position = TILE_POSITION, .source = TILE_SOURCE}};

/** @brief The six corners of the two triangles of a tile, and the first and
 * the third of them: its top left and bottom right corners. */
constexpr std::size_t VERTICES_OF_ONE_TILE = 6;
constexpr std::size_t FIRST_VERTEX = 0;
constexpr std::size_t THIRD_VERTEX = 2;

/** @brief Where the corners of that tile lie in its map. */
constexpr sf::Vector2f TOP_LEFT_IN_THE_MAP{32.0F, 16.0F};
constexpr sf::Vector2f BOTTOM_RIGHT_IN_THE_MAP{48.0F, 32.0F};

/** @brief Where its picture lies in the tileset: its corners, and the corner
 * the top left of the tile takes once mirrored left to right, or top to
 * bottom. */
constexpr sf::Vector2f TOP_LEFT_IN_THE_TILESET{48.0F, 0.0F};
constexpr sf::Vector2f BOTTOM_RIGHT_IN_THE_TILESET{64.0F, 16.0F};
constexpr sf::Vector2f TOP_RIGHT_IN_THE_TILESET{64.0F, 0.0F};
constexpr sf::Vector2f BOTTOM_LEFT_IN_THE_TILESET{48.0F, 16.0F};
