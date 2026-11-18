#include "MapPlane.hpp"
#include <SFML/Graphics/VertexArray.hpp>
#include <cmath>
#include <cstddef>
#include <span>
#include <utility>

namespace rtype::client {

void MapPlane::addMap(sf::VertexArray tiles, int width) {
  firstTileColumns_.push_back(
      tiles.getVertexCount() == 0
          ? width
          : static_cast<int>(std::floor(tiles.getBounds().position.x)));
  maps_.push_back(std::move(tiles));
  widths_.push_back(width);
}

std::span<const int> MapPlane::widths() const { return widths_; }

int MapPlane::firstTileColumnOf(std::size_t map) const {
  return firstTileColumns_.at(map);
}

const sf::VertexArray& MapPlane::tilesOf(std::size_t map) const {
  return maps_.at(map);
}

}  // namespace rtype::client
