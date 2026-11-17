#include "MapTileVertices.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <span>
#include <utility>
#include "MapTile.hpp"
#include "TileCornerConstants.hpp"

namespace rtype::client {

namespace {

using TileCorners = std::array<sf::Vector2f, TILE_CORNERS>;

TileCorners cornersOf(sf::Vector2i origin, int size) {
  const sf::Vector2f topLeft(origin);
  const auto side = static_cast<float>(size);
  TileCorners corners{};
  corners[TOP_LEFT_CORNER] = topLeft;
  corners[TOP_RIGHT_CORNER] = topLeft + sf::Vector2f{side, 0.0F};
  corners[BOTTOM_RIGHT_CORNER] = topLeft + sf::Vector2f{side, side};
  corners[BOTTOM_LEFT_CORNER] = topLeft + sf::Vector2f{0.0F, side};
  return corners;
}

TileCorners textureCornersOf(const MapTile& tile, int size) {
  TileCorners corners = cornersOf(tile.source, size);
  if (tile.flipX) {
    std::swap(corners[TOP_LEFT_CORNER], corners[TOP_RIGHT_CORNER]);
    std::swap(corners[BOTTOM_LEFT_CORNER], corners[BOTTOM_RIGHT_CORNER]);
  }
  if (tile.flipY) {
    std::swap(corners[TOP_LEFT_CORNER], corners[BOTTOM_LEFT_CORNER]);
    std::swap(corners[TOP_RIGHT_CORNER], corners[BOTTOM_RIGHT_CORNER]);
  }
  return corners;
}

}  // namespace

sf::VertexArray tileVertices(std::span<const MapTile> tiles, int tileSize) {
  sf::VertexArray vertices(sf::PrimitiveType::Triangles);
  for (const MapTile& tile : tiles) {
    const TileCorners place = cornersOf(tile.position, tileSize);
    const TileCorners texture = textureCornersOf(tile, tileSize);
    for (const std::size_t corner : TILE_TRIANGLE_CORNERS) {
      vertices.append(sf::Vertex{.position = place.at(corner),
                                 .texCoords = texture.at(corner)});
    }
  }
  return vertices;
}

}  // namespace rtype::client
