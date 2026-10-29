#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "IInterfaceCanvas.hpp"

namespace rtype::client {

/**
 * @brief Paints the four corners of a box: at each, two strokes
 * BOX_CORNER_LENGTH pixels long and one pixel thick, along its edges.
 *
 * The bottom corners stand on the last row of the box, or on its first row
 * for a box less than two rows tall: the corners of a box that closes meet
 * instead of crossing. The squadron of the title frames its ships with them,
 * the help the part it explains.
 */
void paintBoxCorners(IInterfaceCanvas& canvas, sf::IntRect box,
                     sf::Color color);

}  // namespace rtype::client
