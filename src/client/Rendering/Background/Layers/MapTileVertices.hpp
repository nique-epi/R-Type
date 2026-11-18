#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include <span>
#include "MapTile.hpp"

namespace rtype::client {

/** @returns Two triangles per tile of @p tiles, each a square @p tileSize
 * pixels wide at its position, textured from its source in the tileset and
 * mirrored as the tile says. */
[[nodiscard]] sf::VertexArray tileVertices(std::span<const MapTile> tiles,
                                           int tileSize);

}  // namespace rtype::client
