#pragma once

#include <SFML/System/Vector2.hpp>
#include "ArmSide.hpp"

namespace rtype::client {

/** @brief Where an arm is bolted: its base, its side, the side its elbow
 * bends to, 1 or -1, and where it grips the ship, in pixels of the ship
 * frame. */
struct ArmMount {
  sf::Vector2i base;
  ArmSide side{};
  int bend{};
  sf::Vector2i grip;
};

}  // namespace rtype::client
