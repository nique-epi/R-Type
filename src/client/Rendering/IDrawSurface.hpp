#pragma once

#include <string_view>
#include "Position.hpp"
#include "SpriteDrawing.hpp"
#include "TextStyle.hpp"

namespace rtype::client {

/**
 * @brief Where the render system draws. It knows nothing about entities.
 */
class IDrawSurface {
 public:
  virtual ~IDrawSurface() = default;

  /** @brief Draws a rectangle of an image, see SpriteDrawing. */
  virtual void draw(const SpriteDrawing& sprite) = 0;

  /**
   * @brief Writes one line of text with the pixel fonts of the interface,
   * centered on a point.
   *
   * @param text UTF-8 text.
   * @param center Middle of the line, in logical units of the playfield.
   * @param style Font, color and outline; the line is centered whatever its
   * alignment says.
   */
  virtual void write(std::string_view text, game::Position center,
                     TextStyle style) = 0;
};

}  // namespace rtype::client
