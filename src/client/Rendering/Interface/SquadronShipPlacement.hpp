#pragma once

#include <SFML/System/Vector2.hpp>
#include "SquadronShip.hpp"

namespace rtype::client {

/** @brief Where a ship is, from the top-left corner of the box, and whether
 * it has reached its place. */
struct SquadronShipPlacement {
  const SquadronShip* ship{};
  sf::Vector2i topLeft;
  bool landed{};
};

}  // namespace rtype::client
