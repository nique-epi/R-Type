#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cstdint>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief A box whose top-left and bottom-right corners are cut in steps of one
 * pixel: the shape of every bar and panel of the menus.
 *
 * The outer shape, cut over CORNER_CUT_ROWS rows, is painted in the border
 * color; the inside, BORDER_THICKNESS in and cut one row less, over it, which
 * leaves a border along the cuts too. The area must be at least SIDES_PER_AXIS
 * times CORNER_CUT_ROWS tall. A box open at the bottom has only its top-left
 * corner cut and no bottom border, as a tab that opens into the panel under it.
 */
class CutCornerBox {
 public:
  /** @brief Colors of the box: its border, BORDER_THICKNESS thick, and its
   * inside. */
  struct Colors {
    sf::Color border;
    sf::Color inside;
  };

  /** @brief Its edges: closed, or open at the bottom. */
  enum class Shape : std::uint8_t {
    Closed,
    OpenBottom,
  };

  /** @brief A closed box.
   * @param area Where the box lies, in pixels of the interface picture. */
  CutCornerBox(sf::IntRect area, Colors colors);

  CutCornerBox(sf::IntRect area, Colors colors, Shape shape);

  void paint(IInterfaceCanvas& canvas) const;

 private:
  sf::IntRect area_;
  Colors colors_;
  Shape shape_ = Shape::Closed;
};

}  // namespace rtype::client
