#pragma once

#include <optional>
#include "Position.hpp"
#include "Sprite.hpp"

namespace rtype::client {

/** @brief What a ship destroyed leaves to its effects: where it stood, the
 * sprite it showed, and the color of its pilot when known. */
struct DownedShip {
  game::Position center{};
  game::Sprite sprite{};
  std::optional<int> pilotColor;
};

}  // namespace rtype::client
