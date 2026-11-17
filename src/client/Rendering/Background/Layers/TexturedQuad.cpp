#include "TexturedQuad.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include "TileCornerConstants.hpp"

namespace rtype::client {

namespace {

using Corners = std::array<sf::Vector2f, TILE_CORNERS>;

/** @returns The corners of @p rectangle, clockwise from the top left. */
Corners cornersOf(const sf::FloatRect& rectangle) {
  const sf::Vector2f topLeft = rectangle.position;
  const sf::Vector2f size = rectangle.size;
  Corners corners{};
  corners[TOP_LEFT_CORNER] = topLeft;
  corners[TOP_RIGHT_CORNER] = topLeft + sf::Vector2f{size.x, 0.0F};
  corners[BOTTOM_RIGHT_CORNER] = topLeft + size;
  corners[BOTTOM_LEFT_CORNER] = topLeft + sf::Vector2f{0.0F, size.y};
  return corners;
}

}  // namespace

void appendQuad(sf::VertexArray& triangles, const sf::FloatRect& place,
                const sf::FloatRect& texture) {
  const Corners placed = cornersOf(place);
  const Corners textured = cornersOf(texture);
  for (const std::size_t corner : TILE_TRIANGLE_CORNERS) {
    triangles.append(sf::Vertex{.position = placed.at(corner),
                                .texCoords = textured.at(corner)});
  }
}

}  // namespace rtype::client
