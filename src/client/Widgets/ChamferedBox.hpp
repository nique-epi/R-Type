#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/** @brief Colors of a chamfered box: its border, BORDER_THICKNESS thick, and
 * its inside. */
struct ChamferedBoxColors {
  sf::Color border;
  sf::Color inside;
};

/**
 * @brief Paints a box whose top-left and bottom-right corners are cut in
 * steps of one pixel: the shape of every bar and panel of the menus.
 *
 * The outer shape, cut over CHAMFER_ROWS rows, is painted in the border color;
 * the inside, BORDER_THICKNESS in and cut one row less, over it, which leaves a
 * border along the cuts too. The area must be at least SIDES_PER_AXIS times
 * CHAMFER_ROWS tall.
 */
void paintChamferedBox(IInterfaceCanvas& canvas, sf::IntRect area,
                       ChamferedBoxColors colors);

}  // namespace rtype::client
