#include "MapTileVertices.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <span>
#include "MapTile.hpp"
#include "TexturedQuad.hpp"

namespace rtype::client {

namespace {

/** @returns The square of side @p side that @p tile takes in its tileset,
 * its width or its height negative for a tile mirrored on that axis. */
sf::FloatRect textureRectOf(const MapTile& tile, float side) {
  const sf::Vector2f source(tile.source);
  return sf::FloatRect{{tile.flipX ? source.x + side : source.x,
                        tile.flipY ? source.y + side : source.y},
                       {tile.flipX ? -side : side, tile.flipY ? -side : side}};
}

}  // namespace

sf::VertexArray tileVertices(std::span<const MapTile> tiles, int tileSize) {
  const auto side = static_cast<float>(tileSize);
  sf::VertexArray vertices(sf::PrimitiveType::Triangles);
  for (const MapTile& tile : tiles) {
    appendQuad(vertices,
               sf::FloatRect{sf::Vector2f(tile.position), {side, side}},
               textureRectOf(tile, side));
  }
  return vertices;
}

}  // namespace rtype::client
