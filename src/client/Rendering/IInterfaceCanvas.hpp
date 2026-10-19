#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string_view>
#include "TextStyle.hpp"

namespace sf {
class RenderTarget;
}  // namespace sf

namespace rtype::client {

/**
 * @brief The picture of a menu or of another element of the interface,
 * painted in pixels of INTERFACE_AREA, then laid over the playfield.
 *
 * A screen paints it in its update, from clear() to display(), and lays it in
 * its draw. Fonts and textures are found by the canvas: whoever paints never
 * touches the graphics card, so it can be tested without one.
 */
class IInterfaceCanvas {
 public:
  virtual ~IInterfaceCanvas() = default;

  /** @brief Starts a new picture: every pixel becomes transparent. */
  virtual void clear() = 0;

  /** @brief Paints a rectangle of one color over what is painted there; a
   * translucent color lets it show through. */
  virtual void fill(sf::IntRect area, sf::Color color) = 0;

  /**
   * @brief Writes one line of text, on whole pixels.
   *
   * @param text UTF-8 text.
   * @param anchor Top-left corner of the line, its top-right corner with
   * TextAlignment::Right, or the middle of its top edge with
   * TextAlignment::Center.
   */
  virtual void write(std::string_view text, sf::Vector2i anchor,
                     TextStyle style) = 0;

  /**
   * @brief Copies a rectangle of a loaded texture, each of its pixels made a
   * square of scale × scale pixels, cutting off whatever falls outside
   * visibleArea.
   *
   * @param assetId Id of the texture; an id with no loaded texture paints
   * nothing.
   * @param source Rectangle of the texture, in pixels of the texture.
   * @param topLeft Where the top-left pixel of source lands.
   * @param scale Whole factor each pixel of the texture is enlarged by,
   * NATIVE_SCALE for a copy pixel for pixel; a pixel of the texture that
   * visibleArea cuts is left out whole.
   * @param visibleArea The only pixels the copy may paint.
   */
  virtual void drawImage(std::string_view assetId, sf::IntRect source,
                         sf::Vector2i topLeft, int scale,
                         sf::IntRect visibleArea) = 0;

  /** @brief Ends the picture started by clear(): show() lays it until the
   * next display(). */
  virtual void display() = 0;

  /**
   * @brief Lays the last displayed picture over the playfield.
   *
   * @param target Target whose view shows the playfield in its logical units.
   */
  virtual void show(sf::RenderTarget& target) const = 0;
};

}  // namespace rtype::client
