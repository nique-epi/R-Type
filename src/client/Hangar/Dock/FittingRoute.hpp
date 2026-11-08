#pragma once

#include <SFML/System/Vector2.hpp>

namespace rtype::client {

/** @brief The points a fitting arm goes by: where it grips the hull, where
 * it fetches the item off screen, where its jaws set the item down, and how
 * far above (negative) or under it the arm brings the item first. */
struct FittingRoute {
  sf::Vector2i grip;
  sf::Vector2i away;
  sf::Vector2i place;
  int approach{};
};

}  // namespace rtype::client
