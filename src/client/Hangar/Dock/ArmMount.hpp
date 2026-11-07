#pragma once

#include <SFML/System/Vector2.hpp>
#include "ArmSide.hpp"

namespace rtype::client {

/** @brief Where an arm is bolted: its base, its side, and the side its elbow
 * bends to, 1 or -1. */
struct ArmMount {
  sf::Vector2i base;
  ArmSide side{};
  int bend{};
};

}  // namespace rtype::client
