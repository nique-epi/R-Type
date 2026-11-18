#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "StripFrame.hpp"

namespace rtype::client {

/**
 * @brief Where a layer of a background paints, in background pixels. It
 * knows nothing about stars or walls: the pixels come first, then the frames
 * and the tiles in the order they were asked for. LayerCanvas in the client,
 * a double in the tests.
 */
class ILayerCanvas {
 public:
  virtual ~ILayerCanvas() = default;

  /** @brief Paints the pixel @p pixel in @p color. */
  virtual void plot(sf::Vector2i pixel, sf::Color color) = 0;

  /** @brief Paints @p frame with its top-left corner on @p topLeft. A frame
   * of an image that is not loaded paints nothing. */
  virtual void drawFrame(const StripFrame& frame, sf::Vector2i topLeft) = 0;

  /**
   * @brief Paints @p tiles, triangles textured from the image @p tileset,
   * moved @p column columns to the right. Tiles of an image that is not
   * loaded paint nothing.
   */
  virtual void drawTiles(std::string_view tileset, const sf::VertexArray& tiles,
                         int column) = 0;
};

}  // namespace rtype::client
