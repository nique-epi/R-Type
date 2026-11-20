#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>
#include "ColoredPixel.hpp"

namespace rtype::client {

/** @brief What the cashier reads once in the picture of its cage: the gold
 * of the vault, where coins sparkle, and the brass of the grille, painted
 * again in front of the teller. Empty, the cage shows its picture still and
 * the teller in front of the grille. */
struct CashierRoom {
  std::vector<sf::Vector2i> gold;
  std::vector<ColoredPixel> grille;
};

}  // namespace rtype::client
