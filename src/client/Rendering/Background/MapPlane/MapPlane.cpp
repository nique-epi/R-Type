#include "MapPlane.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cmath>
#include <cstddef>
#include <span>
#include <utility>
#include "MapColumn.hpp"
#include "MapColumns.hpp"

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

void MapPlane::draw(sf::RenderTarget& canvas, const sf::Texture& tileset,
                    int firstColumn) const {
  sf::RenderStates states(&tileset);
  for (const MapColumn& placed : mapColumns(firstColumn, widths_)) {
    states.transform = sf::Transform::Identity;
    states.transform.translate({static_cast<float>(placed.column), 0.0F});
    canvas.draw(maps_[placed.map], states);
  }
}

}  // namespace rtype::client
