#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <cstddef>
#include <span>
#include <vector>

namespace rtype::client {

/**
 * @brief One plane of the maps of a biome: the tiles of that plane for every
 * map, built once, and the width of each map, drawn end to end as the biome
 * scrolls in (see mapColumns()).
 */
class MapPlane {
 public:
  /** @brief Adds the next map of the plane, @p width pixels wide. */
  void addMap(sf::VertexArray tiles, int width);

  /** @returns The width of each map, in pixels, in the order they were
   * added. */
  [[nodiscard]] std::span<const int> widths() const;

  /** @returns The tiles of the map @p map, which must have been added. */
  [[nodiscard]] const sf::VertexArray& tilesOf(std::size_t map) const;

  /** @brief Draws the maps that show once the plane entered by
   * @p enteredPixels, textured by @p tileset. */
  void draw(sf::RenderTarget& canvas, const sf::Texture& tileset,
            float enteredPixels) const;

 private:
  std::vector<sf::VertexArray> maps_;
  std::vector<int> widths_;
};

}  // namespace rtype::client
