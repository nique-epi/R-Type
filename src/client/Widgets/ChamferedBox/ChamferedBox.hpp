#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief A box whose top-left and bottom-right corners are cut in steps of one
 * pixel: the shape of every bar and panel of the menus.
 *
 * The outer shape, cut over CHAMFER_ROWS rows, is painted in the border color;
 * the inside, BORDER_THICKNESS in and cut one row less, over it, which leaves a
 * border along the cuts too. The area must be at least SIDES_PER_AXIS times
 * CHAMFER_ROWS tall.
 */
class ChamferedBox {
 public:
  /** @brief Colors of the box: its border, BORDER_THICKNESS thick, and its
   * inside. */
  struct Colors {
    sf::Color border;
    sf::Color inside;
  };

  /** @param area Where the box lies, in pixels of the interface picture. */
  ChamferedBox(sf::IntRect area, Colors colors);

  void paint(IInterfaceCanvas& canvas) const;

 private:
  sf::IntRect area_;
  Colors colors_;
};

}  // namespace rtype::client
